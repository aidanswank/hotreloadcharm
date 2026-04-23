/**
 * SVF Filter Plugin - Implementation of Your Formula
 * 
 * This file documents how your formula is implemented in filter_plugin.cpp
 */

/*
Your original formula:

    float v0, v1;
    
    formula_main {
        float c = pow(0.5, (128-KNOB_1*128)   / 16.0);
        float r = pow(0.5, (KNOB_2*24+24) / 16.0);
        
        v0 =  (1-r*c)*v0  -  (c)*v1  + (c)*input;
        v1 =  (1-r*c)*v1  +  (c)*v0;
        
        return v1;
    }

IMPLEMENTATION:
================

The plugin maintains two state variables per channel:
- FilterPluginState::v0_left  / v0_right
- FilterPluginState::v1_left  / v1_right

Parameters from host:
- context->knob_1 = KNOB_1 (0.0 to 1.0)
- context->knob_2 = KNOB_2 (0.0 to 1.0)

Each sample is processed as:

    // Calculate coefficients from knobs
    float c = std::pow(0.5f, (128.0f - context->knob_1 * 128.0f) / 16.0f);
    float r = std::pow(0.5f, (context->knob_2 * 24.0f + 24.0f) / 16.0f);
    
    // Update state variables (per-sample iteration)
    float input = channel[i];
    v0 = (1.0f - r * c) * v0 - c * v1 + c * input;
    v1 = (1.0f - r * c) * v1 + c * v0;
    
    // Output
    channel[i] = v1;

The filter is a State Variable Filter (SVF) with:
- Cutoff frequency controlled by KNOB_1
- Resonance controlled by KNOB_2
- Lowpass output on v1

COEFFICIENT RANGES:
===================

KNOB_1 (Cutoff Frequency):
- Input range: 0.0 to 1.0
- Exponent range: 128 to 0
- Coefficient c range: 0.5^(128/16) ≈ 0 to 0.5^(0/16) = 1.0
- Lower knob_1 = lower cutoff, higher knob_1 = higher cutoff

KNOB_2 (Resonance):
- Input range: 0.0 to 1.0
- Exponent range: 24 to 48
- Coefficient r range: 0.5^(24/16) ≈ 0.177 to 0.5^(48/16) ≈ 0.031
- Controls filter Q (resonance peak)

USAGE IN PLUGIN:
================

1. Load the plugin:
   app.load_plugin("build/plugins/filter_plugin.dylib");

2. Set parameters via host (KNOB_1 and KNOB_2 controls)

3. Process audio:
   app.process_audio(outputs, num_channels, num_samples);

4. Hot-reload when you modify the formula:
   app.reload_plugin();

EXTENDING THE FORMULA:
======================

To add more parameters (KNOB_3, KNOB_4), modify:

1. PluginAudioContext in plugin_interface.h (already has knob_3 and knob_4)
2. filter_plugin.cpp process_audio function
3. Your custom formula using the new knobs

Example with additional parameter:

    float cutoff_offset = context->knob_3 * 64.0f;  // Additional parameter
    float c = std::pow(0.5f, 
                      (128.0f - context->knob_1 * 128.0f + cutoff_offset) / 16.0f);

Then recompile and hot-reload the plugin!
*/
