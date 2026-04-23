#pragma once

#include "plugin_interface.h"
#include <string>
#include <dlfcn.h>

/**
 * Plugin Loader - Manages hot-reloadable plugins
 * 
 * This class handles:
 * - Loading plugins from dylib files
 * - Hot-reloading (unload old, load new)
 * - Managing plugin lifecycle (create/destroy)
 */

class PluginLoader
{
public:
    PluginLoader() = default;
    
    ~PluginLoader()
    {
        unload();
    }
    
    // Load a plugin from a dylib file
    bool load(const std::string& dylib_path)
    {
        // Unload existing plugin first
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
        this->plugin_create = reinterpret_cast<CreateFunc>(dlsym(this->handle, "plugin_create"));
        this->plugin_destroy = reinterpret_cast<DestroyFunc>(dlsym(this->handle, "plugin_destroy"));
        this->plugin_process = reinterpret_cast<ProcessFunc>(dlsym(this->handle, "plugin_process_audio"));
        this->plugin_get_name = reinterpret_cast<GetNameFunc>(dlsym(this->handle, "plugin_get_name"));
        this->plugin_get_version = reinterpret_cast<GetVersionFunc>(dlsym(this->handle, "plugin_get_version"));
        
        if (!this->plugin_create || !this->plugin_destroy || !this->plugin_process)
        {
            this->error = "Plugin dylib missing required functions";
            dlclose(this->handle);
            this->handle = nullptr;
            return false;
        }
        
        return true;
    }
    
    // Hot-reload: unload and reload the plugin
    bool reload()
    {
        if (this->dylib_path.empty())
        {
            this->error = "No plugin path to reload";
            return false;
        }
        
        // Destroy current plugin state
        if (this->state && this->plugin_destroy)
        {
            this->plugin_destroy(this->state);
            this->state = nullptr;
        }
        
        // Reload dylib
        return load(this->dylib_path);
    }
    
    // Create a new plugin instance
    bool create_instance()
    {
        if (!this->plugin_create)
        {
            this->error = "Plugin not loaded";
            return false;
        }
        
        if (this->state && this->plugin_destroy)
        {
            this->plugin_destroy(this->state);
        }
        
        this->state = this->plugin_create();
        return this->state != nullptr;
    }
    
    // Process audio
    void process_audio(float* outputs[], int num_channels, int num_samples,
                       const PluginAudioContext* context)
    {
        if (this->state && this->plugin_process)
        {
            this->plugin_process(this->state, outputs, num_channels, num_samples, context);
        }
    }
    
    // Get plugin metadata
    const char* get_name() const
    {
        if (this->plugin_get_name)
            return this->plugin_get_name();
        return "Unknown";
    }
    
    const char* get_version() const
    {
        if (this->plugin_get_version)
            return this->plugin_get_version();
        return "Unknown";
    }
    
    bool is_loaded() const { return this->handle != nullptr; }
    const std::string& get_error() const { return this->error; }
    const std::string& get_path() const { return this->dylib_path; }
    
private:
    void unload()
    {
        if (this->state && this->plugin_destroy)
        {
            this->plugin_destroy(this->state);
            this->state = nullptr;
        }
        
        if (this->handle)
        {
            dlclose(this->handle);
            this->handle = nullptr;
        }
        
        this->plugin_create = nullptr;
        this->plugin_destroy = nullptr;
        this->plugin_process = nullptr;
        this->plugin_get_name = nullptr;
        this->plugin_get_version = nullptr;
    }
    
    using CreateFunc = PluginState* (*)();
    using DestroyFunc = void (*)(PluginState*);
    using ProcessFunc = void (*)(PluginState*, float*[], int, int, const PluginAudioContext*);
    using GetNameFunc = const char* (*)();
    using GetVersionFunc = const char* (*)();
    
    void* handle = nullptr;
    PluginState* state = nullptr;
    std::string dylib_path;
    std::string error;
    
    CreateFunc plugin_create = nullptr;
    DestroyFunc plugin_destroy = nullptr;
    ProcessFunc plugin_process = nullptr;
    GetNameFunc plugin_get_name = nullptr;
    GetVersionFunc plugin_get_version = nullptr;
};
