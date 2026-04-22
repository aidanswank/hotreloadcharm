/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

#include "JucePlatformLayer.h"

//==============================================================================
/**
*/
class CharmAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CharmAudioProcessorEditor (CharmAudioProcessor&);
    ~CharmAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CharmAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CharmAudioProcessorEditor)
    
    // Create an instance of the OpenGLComponent.
    JucePlatformLayer juce_platform_layer;
};
