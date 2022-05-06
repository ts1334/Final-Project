/*
  ==============================================================================

    This file is adapted from the basic framework code for a JUCE plugin editor.
    Sections beginning with //================== are generated, adapted, added to 
    or edited from this original auto generated code

    Header file for the editor of a JUCE VST video game sample emulation plugin
    which takes a sample and processes it to sound like a sample from a selected
    console with the selected parameters

  ==============================================================================
*/

// Parameter control and such adapted from:
// [1]
/***********************************************************************************
* Title: SimpleEQ
* Author: matkatmusic
* Date: 16 Apr 2021
* Code Version: Unknown
* Availability: https://github.com/matkatmusic/SimpleEQ
***********************************************************************************/

// File loading and sampler functionality such adapted from:
// [2]
/***********************************************************************************
* Title: helloSampler
* Author: The Audio Programmer (AKA Josh)
* Date: 2020
* Code Version: Unknown
* Availability: https://www.youtube.com/watch?v=F-EkwKFftPY&t=102s and
                https://www.youtube.com/watch?v=2OErY-qhGyw&list=WL&index=2
***********************************************************************************/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

class ProjectCodeAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                         public juce::FileDragAndDropTarget,
                                         juce::AudioProcessorParameter::Listener,
                                         juce::Timer
{
public:
    ProjectCodeAudioProcessorEditor (ProjectCodeAudioProcessor&);
    ~ProjectCodeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    // From [1]
    void parameterValueChanged(int parameterIndex, float newValue) override;            // Called upon change in parameter value
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override   // Called upon change in parameter gesture (unimplemented)
    {
    }
    void timerCallback() override;  // Timer callback!

    // From [2]
    bool isInterestedInFileDrag(const juce::StringArray& files) override;       // Sets whether dragged files are to be taken into account
    void filesDropped(const juce::StringArray& files, int x, int y) override;   // Detects whether files have been dropped

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    ProjectCodeAudioProcessor& audioProcessor;

    // From [1]
    juce::Atomic<bool> parametersChanged = false;

    // From [2]
    juce::TextButton loadButton{ "Drag and Drop or Click to Select an Audio File to be Sampled" };  // A button to bring up file selector for an audio sample to be selected
    
    // General controls
    juce::ComboBox consoleSelector, sampleMIDINoteSelector;

    // NES Controls
    juce::Slider NESBitDepthSlider, NESSampleRateSlider;
    juce::ComboBox PCMorDPCMSelector;

    // SNES Controls
    juce::Slider SNESBitDepthSlider, SNESSampleRateSlider, SNESDPCMSlider;

    // GB Controls

    // GBA Controls

    // From [1]
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    // Attachments to be used to attach parameters to controls
    juce::AudioProcessorValueTreeState::ComboBoxAttachment consoleSelectorAttachment, sampleMIDINoteSelectorAttachment, PCMorDPCMSelectorAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment NESBitDepthSliderAttachment, NESSampleRateSliderAttachment, SNESBitDepthSliderAttachment, SNESSampleRateSliderAttachment, SNESDPCMSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectCodeAudioProcessorEditor)
};
