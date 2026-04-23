#include "plugin_interface.h"
#include <cstring>
#include <cmath>

/**
 * Example Filter Plugin - Resonant low-pass filter using CRTP
 * 
 * This plugin implements a simple resonant filter with two state variables (v0, v1).
 * The filter coefficients are controlled by KNOB_1 and KNOB_2.
 * 
 * Uses static polymorphism via CRTP - state is a member variable, no passing around!
 */

class FilterPlugin : public PluginBase<FilterPlugin>
{
public:
    struct FilterState
    {
        float v0 = 0.0f;
        float v1 = 0.0f;
    };
    FilterState filter_states[2]; // 0 = left, 1 = right
    
    void initialize()
    {
        // Initialize both channel states
        this->filter_states[0] = FilterState();
        this->filter_states[1] = FilterState();
    }
    
    void process_audio(float* outputs[], int num_channels, int num_samples, const PluginAudioContext* context)
    {
        if (!context || num_channels < 1)
            return;
        
        // Calculate filter coefficients from knobs
        // KNOB_1: controls cutoff frequency (0.0 = low, 1.0 = high)
        // KNOB_2: controls resonance (0.0 = no resonance, 1.0 = high resonance)
        float c = std::pow(0.5f, (128.0f - context->knob_1 * 128.0f) / 16.0f);
        float r = std::pow(0.5f, (context->knob_2 * 24.0f + 24.0f) / 16.0f);
        
        // Process left channel
        if (num_channels >= 1)
        {
            float* left = outputs[0];
            auto& left_state = this->filter_states[0];
            
            for (int i = 0; i < num_samples; ++i)
            {
                float input = left[i];
                
                // SVF equations - using member variables directly!
                left_state.v0 = (1.0f - r * c) * left_state.v0 - c * left_state.v1 + c * input;
                left_state.v1 = (1.0f - r * c) * left_state.v1 + c * left_state.v0;
                
                left[i] = left_state.v1;
            }
        }
        
        // Process right channel
        if (num_channels >= 2)
        {
            float* right = outputs[1];
            auto& right_state = this->filter_states[1];
            
            for (int i = 0; i < num_samples; ++i)
            {
                float input = right[i];
                
                // SVF equations - using member variables directly!
                right_state.v0 = (1.0f - r * c) * right_state.v0 - c * right_state.v1 + c * input;
                right_state.v1 = (1.0f - r * c) * right_state.v1 + c * right_state.v0;
                
                right[i] = right_state.v1;
            }
        }
    }
    
    const char* get_name() const
    {
        return "SVF Filter";
    }
    
    const char* get_version() const
    {
        return "1.0.1";
    }
};

extern "C" {

PluginState* plugin_create()
{
    FilterPlugin::instance().initialize();
    return FilterPlugin::instance().get_state();
}

void plugin_destroy(PluginState* state)
{
    // State is managed by the singleton, no need to delete
    // But we could reset state here if needed
    FilterPlugin::instance().initialize();
}

void plugin_process_audio(PluginState* state,
                         float* outputs[],
                         int num_channels,
                         int num_samples,
                         const PluginAudioContext* context)
{
    FilterPlugin::instance().process_audio(outputs, num_channels, num_samples, context);
}

const char* plugin_get_name()
{
    return FilterPlugin::instance().get_name();
}

const char* plugin_get_version()
{
    return FilterPlugin::instance().get_version();
}

} // extern "C"
