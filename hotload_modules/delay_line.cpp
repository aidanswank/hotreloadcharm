#include "module_interface.h"
#include <cstring>
#include <cmath>
#include <algorithm>

/**
 * Example Filter Module - Resonant low-pass filter using CRTP
 * 
 * This module implements a simple resonant filter with two state variables (v0, v1).
 * The filter coefficients are controlled by KNOB_1 and KNOB_2.
 * 
 * Uses static polymorphism via CRTP - state is a member variable, no passing around!
 */

float lerp(float a, float b, float f) 
{
    return (a * (1.0 - f)) + (b * f);
}


class DelayLineModule : public ModuleBase<DelayLineModule>
{
public:
    // struct FilterState
    // {
    //     float v0 = 0.0f;
    //     float v1 = 0.0f;
    // };
    // FilterState filter_states[2]; // 0 = left, 1 = right
    
    static constexpr int MAX_DELAY_SAMPLES = 480000; // 10 second at 48kHz

    float buffer[MAX_DELAY_SAMPLES];
    int write_index = 0;
    int read_index = 0;

    char text_buffer[256] = {0};
    int text_cursor_pos = 0;
    int delay_samples = 0;
    bool is_being_edited = false;
    float smoothed_knob_1 = 0.0f;

    void initialize()
    {
        memset(buffer, 0, sizeof(buffer));
    }
    
    void process_audio(float* outputs[], int num_channels, int num_samples, const ModuleAudioContext* context)
    {
        if (!context || num_channels < 1)
            return;
        
            for(int i = 0; i < num_samples; ++i)
            {
                float left_sample = outputs[0][i];
                float right_sample = outputs[1][i];

                float in = left_sample; // for now
                buffer[write_index] = in;

                // Default to text-input value
                // int effective_delay = delay_samples;

                // Override with CV input if connected
                // if (input_connected[TIME_INPUT]) {
                
                smoothed_knob_1 = lerp(smoothed_knob_1, context->knob_1, 0.0001f); // smooth knob changes for demo

                float ms = context->note_duration_ms;
        
        // Convert: samples = (milliseconds / 1000) * sample_rate
                float samples = (ms / 1000.0f) * context->sample_rate;

                int modulated_delay = static_cast<int>((smoothed_knob_1*8) * samples); // modulate delay with knob for demo
                int effective_delay = std::clamp(modulated_delay, 0, MAX_DELAY_SAMPLES - 1);
                // }

                read_index = write_index - effective_delay;
                if (read_index < 0)
                    read_index += MAX_DELAY_SAMPLES;

                // outputs[SIGNAL_OUTPUT] = buffer[read_index];

                write_index = (write_index + 1) % MAX_DELAY_SAMPLES;

                outputs[0][i] = buffer[read_index];
                outputs[1][i] = buffer[read_index];
  
        }
    }
    
    const char* get_name() const
    {
        return "Delay Line";
    }
    
    const char* get_version() const
    {
        return "1.0.0";
    }
};

extern "C" {

ModuleState* module_create()
{
    DelayLineModule::instance().initialize();
    return DelayLineModule::instance().get_state();
}

void module_destroy(ModuleState* state)
{
    // State is managed by the singleton, no need to delete
    // But we could reset state here if needed
    DelayLineModule::instance().initialize();
}

void module_process_audio(ModuleState* state,
                         float* outputs[],
                         int num_channels,
                         int num_samples,
                         const ModuleAudioContext* context)
{
    DelayLineModule::instance().process_audio(outputs, num_channels, num_samples, context);
}

const char* module_get_name()
{
    return DelayLineModule::instance().get_name();
}

const char* module_get_version()
{
    return DelayLineModule::instance().get_version();
}

} // extern "C"
