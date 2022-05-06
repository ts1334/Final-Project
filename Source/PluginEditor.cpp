/*
  ==============================================================================

    This file is adapted from the basic framework code for a JUCE plugin editor.
    Sections beginning with //================== are generated, adapted, added to
    or edited from this original auto generated code

    Implementation file for the editor of a JUCE VST video game sample emulation plugin
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

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ProjectCodeAudioProcessorEditor::ProjectCodeAudioProcessorEditor (ProjectCodeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    // Attach each parameter to the respective user control
    consoleSelectorAttachment(audioProcessor.apvts, "Console", consoleSelector),                        
    sampleMIDINoteSelectorAttachment(audioProcessor.apvts, "SampleMidiNote", sampleMIDINoteSelector),   
    NESBitDepthSliderAttachment(audioProcessor.apvts, "NESBitDepth", NESBitDepthSlider),                
    NESSampleRateSliderAttachment(audioProcessor.apvts, "NESSampleRate", NESSampleRateSlider), 
    PCMorDPCMSelectorAttachment(audioProcessor.apvts, "PCMorDPCM", PCMorDPCMSelector),
    SNESBitDepthSliderAttachment(audioProcessor.apvts, "SNESBitDepth", SNESBitDepthSlider),
    SNESSampleRateSliderAttachment(audioProcessor.apvts, "SNESSampleRate", SNESSampleRateSlider),
    SNESDPCMSliderAttachment(audioProcessor.apvts, "SNESDPCMBit", SNESDPCMSlider)
{
    // From [2]
    loadButton.onClick = [&]() { audioProcessor.loadSample(); };    // Run the loadSample() function from audioProcessor when clicked
    addAndMakeVisible(loadButton);                                  // Add the file load button to the GUI

    // Control adding adapted from [1]
    addAndMakeVisible(consoleSelector);                                                     // Add the console selector to the GUI
    consoleSelector.addItemList(juce::StringArray("NES", "SNES", "GameBoy", "GBA"), 1);     // Fill the GUI component with the console options
    consoleSelector.setSelectedId(1);                                                       // Set initial selection to first option (NES)

    addAndMakeVisible(sampleMIDINoteSelector);  // Add the MIDI note selector to the GUI
    // Store integers 12 to 128 in a string array
    juce::StringArray midiNotesStringArray;     
    for (int i = 12; i <= 128; i++)
    {
        midiNotesStringArray.add((juce::String)i);
    }
    sampleMIDINoteSelector.addItemList(midiNotesStringArray, 1);    // Add the new string array to the GUI component, (options of MIDI notes 12 to 128)
    sampleMIDINoteSelector.setSelectedId(49);                       // Set initial selection to 49th option (MIDI note 60)

    // NES controls made visible first as NES is selected as initial console
    addAndMakeVisible(NESBitDepthSlider);                               // Add NES bit depth slider to the GUI
    addAndMakeVisible(NESSampleRateSlider);                             // Add NES sample rate slider to the GUI
    addAndMakeVisible(PCMorDPCMSelector);                               // Add the PCM/DPCM selector to the GUI
    PCMorDPCMSelector.addItemList(juce::StringArray("PCM", "DPCM"), 1); // Fill PCM/DPCM GUI component with options
    PCMorDPCMSelector.setSelectedId(1);                                 // Set initial selection to the first option (PCM)

    // SNES controls added but not made visible in the GUI as NES is the intially selected console
    addChildComponent(SNESBitDepthSlider);
    addChildComponent(SNESSampleRateSlider);    
    addChildComponent(SNESDPCMSlider);          

    // Listener for each parameter to check when it changes, from from [1]
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }

    // From [1]
    startTimerHz(60);

    setSize (800, 450); // Set the resolution of the VST's window
}

ProjectCodeAudioProcessorEditor::~ProjectCodeAudioProcessorEditor()
{
    // Remove listeners for the parameters when the VST is exited, from [1]
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
    
    // Adapted from [1] and [2]:

    // Set general controls' positions on GUI
    loadButton.setBounds(0, 0, getWidth() / 4, getHeight() / 4);
    consoleSelector.setBounds(getWidth() / 2 - 50, getHeight()/6 - 25, 100, 50);
    sampleMIDINoteSelector.setBounds(getWidth() / 2 - 50, 2*getHeight()/6 - 25, 100, 50);

    // Set NES controls' positions on GUI
    NESBitDepthSlider.setBounds(getWidth() / 2 - 100, 3 * getHeight() / 6 - 50, 200, 100);
    NESSampleRateSlider.setBounds(getWidth() / 2 - 100, 4 * getHeight() / 6 - 50, 200, 100);
    PCMorDPCMSelector.setBounds(getWidth() / 2 - 50, 5 * getHeight() / 6 - 25, 100, 50);

    // Set SNES controls' positions on GUI
    SNESBitDepthSlider.setBounds(getWidth() / 2 - 100, 3 * getHeight() / 6 - 50, 200, 100);
    SNESSampleRateSlider.setBounds(getWidth() / 2 - 100, 4 * getHeight() / 6 - 50, 200, 100);
    SNESDPCMSlider.setBounds(getWidth() / 2 - 100, 5 * getHeight() / 6 - 50, 200, 100);
}

// From [1] Function to called if parameter value has changed
void ProjectCodeAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue) 
{
    parametersChanged.set(true);    // Set parameter changed to true
}

// From [1]
void ProjectCodeAudioProcessorEditor::timerCallback()
{
    // Check parameters changed value is true and if it is set it back to false
    if (parametersChanged.compareAndSetBool(false, true))
    {
        juce::String item = consoleSelector.getItemText(consoleSelector.getSelectedItemIndex());    // Get the currently selected console

        // If NES, make NES controls visible, and other console specific controls invisible
        if (item == "NES")
        {
            NESBitDepthSlider.setVisible(true);
            NESSampleRateSlider.setVisible(true);
            PCMorDPCMSelector.setVisible(true);

            SNESBitDepthSlider.setVisible(false);
            SNESSampleRateSlider.setVisible(false);
            SNESDPCMSlider.setVisible(false);
        }

        // If SNES, make SNES controls visible, and other console specific controls invisible
        if (item == "SNES")
        {
            NESBitDepthSlider.setVisible(false);
            NESSampleRateSlider.setVisible(false);
            PCMorDPCMSelector.setVisible(false);

            SNESBitDepthSlider.setVisible(true);
            SNESSampleRateSlider.setVisible(true);
            SNESDPCMSlider.setVisible(true);
        }

        // If GameBoy, make GameBoy controls visible, and other console specific controls invisible
        if (item == "GameBoy")
        {
            NESBitDepthSlider.setVisible(false);
            NESSampleRateSlider.setVisible(false);
            PCMorDPCMSelector.setVisible(false);

            SNESBitDepthSlider.setVisible(false);
            SNESSampleRateSlider.setVisible(false);
            SNESDPCMSlider.setVisible(false);
        }

        // If GBA, make GBA controls visible, and other console specific controls invisible
        if (item == "GBA")
        {
            NESBitDepthSlider.setVisible(false);
            NESSampleRateSlider.setVisible(false);
            PCMorDPCMSelector.setVisible(false);

            SNESBitDepthSlider.setVisible(false);
            SNESSampleRateSlider.setVisible(false);
            SNESDPCMSlider.setVisible(false);
        }

        // Check that there is a sample loaded into the VST
        if (audioProcessor.sampleLoaded())
        {
            audioProcessor.getAndSetParams();                       // Update the parameters if there is a sample
            audioProcessor.updateSample(audioProcessor.getRange()); // Update the sample based on these parameters 
        }
    }
}

//Drag File from [2]
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

//Drop File from [2]
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
