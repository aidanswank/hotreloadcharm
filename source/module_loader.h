#pragma once

#include "module_interface.h"
#include <string>
#include <dlfcn.h>
#include <filesystem>
#include <unistd.h>


class ModuleLoader
{
public:
    ModuleLoader() = default;
    
    ~ModuleLoader()
    {
        unload();
    }
    
    bool load(const std::string& dylib_path)
    {
        if (this->handle)
            unload();
        
        // Make a unique temp copy so each instance gets its own DSO,
        // preventing the OS from returning the same dlopen handle
        std::string tmp = dylib_path + ".tmp_"
                          + std::to_string(getpid()) + "_"
                          + std::to_string(reinterpret_cast<uintptr_t>(this))
                          + ".dylib";

        std::error_code ec;
        std::filesystem::copy_file(dylib_path,
                                   tmp,
                                   std::filesystem::copy_options::overwrite_existing,
                                   ec);
        if (ec)
        {
            this->error = "Failed to copy dylib for isolation: " + ec.message();
            return false;
        }

        this->handle = dlopen(tmp.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!this->handle)
        {
            this->error = std::string("Failed to load dylib: ") + dlerror();
            std::filesystem::remove(tmp);
            return false;
        }

        this->dylib_path = dylib_path; // keep original path for reload
        this->temp_path  = tmp;        // track temp copy for cleanup
        
        this->module_create      = reinterpret_cast<CreateFunc>(dlsym(this->handle, "module_create"));
        this->module_destroy     = reinterpret_cast<DestroyFunc>(dlsym(this->handle, "module_destroy"));
        this->module_process     = reinterpret_cast<ProcessFunc>(dlsym(this->handle, "module_process_audio"));
        this->module_get_name    = reinterpret_cast<GetNameFunc>(dlsym(this->handle, "module_get_name"));
        this->module_get_version = reinterpret_cast<GetVersionFunc>(dlsym(this->handle, "module_get_version"));
        
        if (!this->module_create || !this->module_destroy || !this->module_process)
        {
            this->error = "Module dylib missing required functions";
            dlclose(this->handle);
            this->handle = nullptr;
            std::filesystem::remove(tmp);
            this->temp_path.clear();
            return false;
        }
        
        return true;
    }
    
    bool reload()
    {
        if (this->dylib_path.empty())
        {
            this->error = "No module path to reload";
            return false;
        }
        
        if (this->state && this->module_destroy)
        {
            this->module_destroy(this->state);
            this->state = nullptr;
        }
        
        return load(this->dylib_path);
    }
    
    bool create_instance()
    {
        if (!this->module_create)
        {
            this->error = "Module not loaded";
            return false;
        }
        
        if (this->state && this->module_destroy)
        {
            this->module_destroy(this->state);
        }
        
        this->state = this->module_create();
        return this->state != nullptr;
    }
    
    void process_audio(float* outputs[], int num_channels, int num_samples,
                       const ModuleAudioContext* context)
    {
        if (this->state && this->module_process)
        {
            this->module_process(this->state, outputs, num_channels, num_samples, context);
        }
    }
    
    const char* get_name() const
    {
        if (this->module_get_name)
            return this->module_get_name();
        return "Unknown";
    }
    
    const char* get_version() const
    {
        if (this->module_get_version)
            return this->module_get_version();
        return "Unknown";
    }
    
    bool is_loaded() const { return this->handle != nullptr; }
    const std::string& get_error() const { return this->error; }
    const std::string& get_path() const { return this->dylib_path; }
    
private:
    void unload()
    {
        if (this->state && this->module_destroy)
        {
            this->module_destroy(this->state);
            this->state = nullptr;
        }
        
        if (this->handle)
        {
            dlclose(this->handle);
            this->handle = nullptr;
        }

        // Remove temp copy now that the handle is closed
        if (!this->temp_path.empty())
        {
            std::filesystem::remove(this->temp_path);
            this->temp_path.clear();
        }
        
        this->module_create      = nullptr;
        this->module_destroy     = nullptr;
        this->module_process     = nullptr;
        this->module_get_name    = nullptr;
        this->module_get_version = nullptr;
    }
    
    using CreateFunc     = ModuleState* (*)();
    using DestroyFunc    = void (*)(ModuleState*);
    using ProcessFunc    = void (*)(ModuleState*, float*[], int, int, const ModuleAudioContext*);
    using GetNameFunc    = const char* (*)();
    using GetVersionFunc = const char* (*)();
    
    void* handle       = nullptr;
    ModuleState* state = nullptr;
    std::string dylib_path; // original path, preserved across reloads
    std::string temp_path;  // unique temp copy, cleaned up on unload
    std::string error;
    
    CreateFunc     module_create      = nullptr;
    DestroyFunc    module_destroy     = nullptr;
    ProcessFunc    module_process     = nullptr;
    GetNameFunc    module_get_name    = nullptr;
    GetVersionFunc module_get_version = nullptr;
};