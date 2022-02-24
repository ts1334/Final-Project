/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ProjectCodeAudioProcessorEditor::ProjectCodeAudioProcessorEditor (ProjectCodeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    bitDepthSliderAttachment(audioProcessor.apvts, "BitDepth", bitDepthSlider)
{
    // Reference
    loadButton.onClick = [&]() { audioProcessor.loadSample(); };
    addAndMakeVisible(loadButton);
    addAndMakeVisible(bitDepthSlider);

    setSize (800, 450);
}

ProjectCodeAudioProcessorEditor::~ProjectCodeAudioProcessorEditor()
{
}

//==============================================================================
void ProjectCodeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black); 
}

void ProjectCodeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // Reference
    loadButton.setBounds(0, 0, getWidth()/4, getHeight()/4);

    bitDepthSlider.setBounds(getWidth() / 2 - 100, getHeight() / 2 - 100, 200, 200);
}

// Reference
//Drag File
bool ProjectCodeAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto file : files)
    {
        if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif"))
        {
            return true;
        }
    }
    return false;
}

// Reference
//Drop File
void ProjectCodeAudioProcessorEditor::filesDropped(const juce::StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(file))
        {
            // Load the file!
            audioProcessor.loadSample(file);
        }
    }

    repaint();
}
