#pragma once 

// loaded from a dylib

struct HotReloadDsp
{
    // put state returned from get_host_parameter
    // then later call hotreload_process_audio() function
    float KNOB_1 = 0.0f;
    float KNOB_2 = 0.0f;
    float KNOB_4 = 0.0f;
    float KNOB_5 = 0.0f;

    // where plugin specific data can go, like filter states
    void *user_data = nullptr;
};

/*
    in app.process_audio()
    {
        for (int i = 0; i < num_samples; ++i)
        {
            // to be called inside the loop
            hotreload_process_audio(state, audio, num_channels);
        }
    }
*/

// state for plugin, array of samples for each channel at that moment in time [0] = L [1] = R 
void hotreload_process_audio(HotReloadDsp* dsp, float* outputs[], int num_channels);