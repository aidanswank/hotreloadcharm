#include "plugin_interface.h"
#include <cstring>
#include <cmath>

/**
 * Simple Gain Plugin - Multiplies audio by a gain factor using CRTP
 *
 * This plugin demonstrates a basic gain effect controlled by KNOB_1.
 * State is now a member variable - no more passing around!
 */

class GainPlugin : public PluginBase<GainPlugin>
{
public:
    // No additional state needed for simple gain
    
    void process_audio(float* outputs[], int num_channels, int num_samples, const PluginAudioContext* context)
    {
        if (!context) return;

        // KNOB_1 controls gain from 0.0 to 2.0
        float gain = context->knob_1 * 2.0f;

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

PluginState* plugin_create()
{
    // Initialize is empty for gain plugin
    return GainPlugin::instance().get_state();
}

void plugin_destroy(PluginState* state)
{
    // State is managed by singleton
}

void plugin_process_audio(PluginState* state,
                         float* outputs[],
                         int num_channels,
                         int num_samples,
                         const PluginAudioContext* context)
{
    GainPlugin::instance().process_audio(outputs, num_channels, num_samples, context);
}

const char* plugin_get_name()
{
    return GainPlugin::instance().get_name();
}

const char* plugin_get_version()
{
    return GainPlugin::instance().get_version();
}

} // extern "C"