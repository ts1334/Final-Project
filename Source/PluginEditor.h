/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/*
// Reference
struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};
*/

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

    //Reference EQ vid
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override
    {
    }
    void timerCallback() override;

    // Reference
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    ProjectCodeAudioProcessor& audioProcessor;

    // Reference EQ
    juce::Atomic<bool> parametersChanged = false;

    // Reference
    juce::TextButton loadButton{ "Drag and Drop or Click to Select an Audio File to be Sampled" };
    
    
    juce::ComboBox consoleSelector, sampleMIDINoteSelector;
    juce::Slider NESBitDepthSlider, NESSampleRateSlider;


    // Reference
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    juce::AudioProcessorValueTreeState::ComboBoxAttachment consoleSelectorAttachment, sampleMIDINoteSelectorAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment NESBitDepthSliderAttachment, NESSampleRateSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectCodeAudioProcessorEditor)
};
