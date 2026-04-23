#pragma once

#include "module_interface.h"
#include <string>
#include <dlfcn.h>

/**
 * Module Loader - Manages hot-reloadable modules
 * 
 * This class handles:
 * - Loading modules from dylib files
 * - Hot-reloading (unload old, load new)
 * - Managing module lifecycle (create/destroy)
 */

class ModuleLoader
{
public:
    ModuleLoader() = default;
    
    ~ModuleLoader()
    {
        unload();
    }
    
    // Load a module from a dylib file
    bool load(const std::string& dylib_path)
    {
        // Unload existing module first
        if (this->handle)
            unload();
        
        // Load the dylib
        this->handle = dlopen(dylib_path.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (!this->handle)
        {
            this->error = std::string("Failed to load dylib: ") + dlerror();
            return false;
        }
        
        this->dylib_path = dylib_path;
        
        // Load function pointers
        this->module_create = reinterpret_cast<CreateFunc>(dlsym(this->handle, "module_create"));
        this->module_destroy = reinterpret_cast<DestroyFunc>(dlsym(this->handle, "module_destroy"));
        this->module_process = reinterpret_cast<ProcessFunc>(dlsym(this->handle, "module_process_audio"));
        this->module_get_name = reinterpret_cast<GetNameFunc>(dlsym(this->handle, "module_get_name"));
        this->module_get_version = reinterpret_cast<GetVersionFunc>(dlsym(this->handle, "module_get_version"));
        
        if (!this->module_create || !this->module_destroy || !this->module_process)
        {
            this->error = "Module dylib missing required functions";
            dlclose(this->handle);
            this->handle = nullptr;
            return false;
        }
        
        return true;
    }
    
    // Hot-reload: unload and reload the module
    bool reload()
    {
        if (this->dylib_path.empty())
        {
            this->error = "No module path to reload";
            return false;
        }
        
        // Destroy current module state
        if (this->state && this->module_destroy)
        {
            this->module_destroy(this->state);
            this->state = nullptr;
        }
        
        // Reload dylib
        return load(this->dylib_path);
    }
    
    // Create a new module instance
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
    
    // Process audio
    void process_audio(float* outputs[], int num_channels, int num_samples,
                       const ModuleAudioContext* context)
    {
        if (this->state && this->module_process)
        {
            this->module_process(this->state, outputs, num_channels, num_samples, context);
        }
    }
    
    // Get module metadata
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
        
        this->module_create = nullptr;
        this->module_destroy = nullptr;
        this->module_process = nullptr;
        this->module_get_name = nullptr;
        this->module_get_version = nullptr;
    }
    
    using CreateFunc = ModuleState* (*)();
    using DestroyFunc = void (*)(ModuleState*);
    using ProcessFunc = void (*)(ModuleState*, float*[], int, int, const ModuleAudioContext*);
    using GetNameFunc = const char* (*)();
    using GetVersionFunc = const char* (*)();
    
    void* handle = nullptr;
    ModuleState* state = nullptr;
    std::string dylib_path;
    std::string error;
    
    CreateFunc module_create = nullptr;
    DestroyFunc module_destroy = nullptr;
    ProcessFunc module_process = nullptr;
    GetNameFunc module_get_name = nullptr;
    GetVersionFunc module_get_version = nullptr;
};
