#pragma once

#include <cstdint>

/**
 * Module Interface - Contract for hot-reloadable DSP modules
 * Modules are compiled as dynamic libraries and can be swapped at runtime
 */

struct ModuleState
{
    // Module-specific state data
    void* user_data = nullptr;
};

struct ModuleAudioContext
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
 * CRTP Base for Module Implementation
 * Provides static polymorphism without virtual functions
 */
template<typename Derived>
class ModuleBase
{
protected:
    ModuleState state;
    
public:
    // Module implementation methods (to be overridden in derived class)
    void initialize() {}
    void process_audio(float* outputs[], int num_channels, int num_samples, const ModuleAudioContext* context) {}
    const char* get_name() const { return "Unknown Module"; }
    const char* get_version() const { return "1.0.0"; }
    
    // Singleton access
    static Derived& instance()
    {
        static Derived inst;
        return inst;
    }
    
    // Get state for external access
    ModuleState* get_state() { return &this->state; }
};

/**
 * Module Interface - All modules must implement these functions
 * These are the dynamic loading entry points
 */

// Initialize module state. Called once at startup.
// Returns a handle to module state, or nullptr on failure
extern "C" ModuleState* module_create();

// Destroy module state. Called at shutdown or when reloading.
extern "C" void module_destroy(ModuleState* state);

// Process audio block. Called for each audio buffer.
// outputs: array of channel pointers, each pointing to num_samples floats
// context: parameter and audio context information
extern "C" void module_process_audio(ModuleState* state,
                                     float* outputs[],
                                     int num_channels,
                                     int num_samples,
                                     const ModuleAudioContext* context);

// Optional: Get module metadata
extern "C" const char* module_get_name();
extern "C" const char* module_get_version();
