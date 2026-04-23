#pragma once

#include <cstdint>

/**
 * Plugin Interface - Contract for hot-reloadable DSP plugins
 * Plugins are compiled as dynamic libraries and can be swapped at runtime
 */

struct PluginState
{
    // Plugin-specific state data
    void* user_data = nullptr;
};

struct PluginAudioContext
{
    // Parameter values passed from the host
    float knob_1 = 0.0f;
    float knob_2 = 0.0f;
    float knob_3 = 0.0f;
    float knob_4 = 0.0f;
    
    // Audio context
    int num_channels = 2;
    int sample_rate = 44100;
};

/**
 * CRTP Base for Plugin Implementation
 * Provides static polymorphism without virtual functions
 */
template<typename Derived>
class PluginBase
{
protected:
    PluginState state;
    
public:
    // Plugin implementation methods (to be overridden in derived class)
    void initialize() {}
    void process_audio(float* outputs[], int num_channels, int num_samples, const PluginAudioContext* context) {}
    const char* get_name() const { return "Unknown Plugin"; }
    const char* get_version() const { return "1.0.0"; }
    
    // Singleton access
    static Derived& instance()
    {
        static Derived inst;
        return inst;
    }
    
    // Get state for external access
    PluginState* get_state() { return &this->state; }
};

/**
 * Plugin Interface - All plugins must implement these functions
 * These are the dynamic loading entry points
 */

// Initialize plugin state. Called once at startup.
// Returns a handle to plugin state, or nullptr on failure
extern "C" PluginState* plugin_create();

// Destroy plugin state. Called at shutdown or when reloading.
extern "C" void plugin_destroy(PluginState* state);

// Process audio block. Called for each audio buffer.
// outputs: array of channel pointers, each pointing to num_samples floats
// context: parameter and audio context information
extern "C" void plugin_process_audio(PluginState* state,
                                     float* outputs[],
                                     int num_channels,
                                     int num_samples,
                                     const PluginAudioContext* context);

// Optional: Get plugin metadata
extern "C" const char* plugin_get_name();
extern "C" const char* plugin_get_version();
