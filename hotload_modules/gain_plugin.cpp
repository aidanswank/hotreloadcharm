#include "module_interface.h"
#include <cstring>
#include <cmath>

/**
 * Simple Gain Module - Multiplies audio by a gain factor using CRTP
 *
 * This module demonstrates a basic gain effect controlled by KNOB_1.
 * State is now a member variable - no more passing around!
 */

class GainModule : public ModuleBase<GainModule>
{
public:
    // No additional state needed for simple gain
    
    void process_audio(float* outputs[], int num_channels, int num_samples, const ModuleAudioContext* context)
    {
        if (!context) return;

        // KNOB_1 controls gain from 0.0 to 2.0
        float gain = context->knob_1 * 4.0f;

        for (int ch = 0; ch < num_channels; ++ch)
        {
            float* channel = outputs[ch];
            for (int i = 0; i < num_samples; ++i)
            {
                channel[i] *= gain;
            }
        }
    }
    
    const char* get_name() const
    {
        return "Simple Gain";
    }
    
    const char* get_version() const
    {
        return "1.0.0";
    }
};

extern "C" {

ModuleState* module_create()
{
    // Initialize is empty for gain module
    return GainModule::instance().get_state();
}

void module_destroy(ModuleState* state)
{
    // State is managed by singleton
}

void module_process_audio(ModuleState* state,
                         float* outputs[],
                         int num_channels,
                         int num_samples,
                         const ModuleAudioContext* context)
{
    GainModule::instance().process_audio(outputs, num_channels, num_samples, context);
}

const char* module_get_name()
{
    return GainModule::instance().get_name();
}

const char* module_get_version()
{
    return GainModule::instance().get_version();
}

} // extern "C"