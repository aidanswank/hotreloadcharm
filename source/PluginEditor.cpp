/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CharmAudioProcessorEditor::CharmAudioProcessorEditor (CharmAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), juce_platform_layer(p.app.get()) // <- pass pointer here
{
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize (400, 300);
  
  
  // Add the child Component and make it visible.
  addAndMakeVisible(juce_platform_layer);
  
  // Normally I would do this in the resized() method, but for the sake of
  // keeping things compact, I'll set the bounds here.
  juce_platform_layer.setBounds(0, 0, 400, 300);

      // Enable resizing
  setResizable(true, true); // (isResizable, useCornerResizer)
//  setResizeLimits(400, 300, 1200, 1000); // minWidth, minHeight, maxWidth, maxHeight
    // printf("hey there!\n");
}

CharmAudioProcessorEditor::~CharmAudioProcessorEditor()
{
}

//==============================================================================
void CharmAudioProcessorEditor::paint (juce::Graphics& g)
{
    // opengl will handle the painting
    // shouldnt see this

    // // (Our component is opaque, so we must completely fill the background with a solid colour)
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void CharmAudioProcessorEditor::resized()
{
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..

  juce_platform_layer.setBounds(getLocalBounds());
}
