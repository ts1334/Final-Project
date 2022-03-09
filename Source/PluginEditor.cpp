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
    consoleSelectorAttachment(audioProcessor.apvts, "Console", consoleSelector),
    sampleMIDINoteSelectorAttachment(audioProcessor.apvts, "SampleMidiNote", sampleMIDINoteSelector),
    NESBitDepthSliderAttachment(audioProcessor.apvts, "NESBitDepth", NESBitDepthSlider),
    NESSampleRateSliderAttachment(audioProcessor.apvts, "NESSampleRate", NESSampleRateSlider)
{
    // Reference
    loadButton.onClick = [&]() { audioProcessor.loadSample(); };
    addAndMakeVisible(loadButton);

    addAndMakeVisible(consoleSelector);
    consoleSelector.addItemList(juce::StringArray("NES", "SNES", "GameBoy", "GBA"), 1);
    addAndMakeVisible(sampleMIDINoteSelector);
    juce::StringArray midiNotesStringArray;
    for (int i = 12; i <= 128; i++)
    {
        midiNotesStringArray.add((juce::String)i);
    }
    sampleMIDINoteSelector.addItemList(midiNotesStringArray, 1);
    addAndMakeVisible(NESBitDepthSlider);
    addAndMakeVisible(NESSampleRateSlider);

    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }

    //Reference EQ Vid
    startTimerHz(60);

    setSize (800, 450);
}

ProjectCodeAudioProcessorEditor::~ProjectCodeAudioProcessorEditor()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }
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
    consoleSelector.setBounds(getWidth() / 2 - 50, getHeight()/5, 100, 50);
    sampleMIDINoteSelector.setBounds(getWidth() / 2 - 50, 2*getHeight()/5, 100, 50);
    NESBitDepthSlider.setBounds(getWidth() / 2 - 100, 3 * getHeight() / 5, 200, 100);
    NESSampleRateSlider.setBounds(getWidth() / 2 - 100, 4 * getHeight() / 5, 200, 100);
}

//Reference EQ
void ProjectCodeAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue) 
{
    parametersChanged.set(true);
}

// Reference EQ
void ProjectCodeAudioProcessorEditor::timerCallback()
{
    if (parametersChanged.compareAndSetBool(false, true))
    {
        if (audioProcessor.sampleLoaded())
        {
            audioProcessor.getAndSetParams();
            audioProcessor.updateSample(audioProcessor.getRange());
        }
    }
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
