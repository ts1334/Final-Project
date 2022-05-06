/*
  ==================================================================================

    This file is adapted from the basic framework code for a JUCE plugin processor.
    Sections beginning with //================== are generated, adapted, added to
    or edited from this original auto generated code

    Header file for the processor of a JUCE VST video game sample emulation plugin 
    which takes a sample and processes it to sound like a sample from a selected 
    console with the selected parameters

  ==================================================================================
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

// Adapted from [1]. Used to store the current values of the parameters that the user can control
struct Parameters
{
    juce::String console = "NES";   // The currently selected console's sampking to be emulated
    bool DPCM = false;              // Whether DPCM is being used
    int DPCMBit = 1;                // The bit size of the DPCM
    int sampleMIDINote = 60;        // The MIDI Note the original audio is played at
    int bitDepth = 16;              // Number of bits that would represent the amplitude to be emulated
    float sampleRate = 44100;       // Sample rate to be emulated
};

//==============================================================================
/**
*/
class ProjectCodeAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ProjectCodeAudioProcessor();
    ~ProjectCodeAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Function to store the current values of the parameter controls
    void getAndSetParams();

    juce::BigInteger getRange();

    // Reference
    // From [2] Loads an audio sample's data, either from drag and drop or selecting in file browser
    void loadSample();
    void loadSample(const juce::String& path);

    // Function to check if a sample is loaded and return result (true or false)
    bool sampleLoaded();

    // Updates the VST's current sample to a new updated one
    void updateSample(juce::BigInteger range);

    // Bit depth conversion functions
    void convertSampleBitDepthDPCM(juce::AudioBuffer<float>* sampleData, float sampleRateConverted, int desiredBitDepth, int slopeBitDepth);
    void convertSampleBitDepthPCM(juce::AudioBuffer<float>* sampleData, int desiredBitDepth);

    // Sample rate conversion function
    void convertSampleSampleRate(juce::AudioBuffer<float>* sampleData, float desiredSampleRate);

    // Higher level bit crush function for processing the sample data
    void bitCrushSample(juce::AudioBuffer<float>* sampleData, float desiredSampleRate, int desiredBitDepth, bool DPCM, int DPCMDepth = 0);

    // Reference
    // From [1] AudioProcessorValueTreeState to store the parameters in
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    // Adapted from [2]
    juce::Synthesiser sampler;                      // Sampler object
    juce::AudioSampleBuffer* originalSampleData;    // Object containing data of the original, unprocessed sample
    juce::File sampleFile;                          // The file containing the original sample 
    juce::BigInteger range;                         // Range of MIDI notes playable by sampler
    const int numVoices{ 1 };                       // Number of voices (set to one so is monophonic)

    Parameters params;  // Current value of parameters object

    juce::AudioFormatManager formatManager;             // Manages the format of the file and can be used to create a reader
    juce::AudioFormatReader* formatReader{ nullptr };   // Reads file of a certain format

    juce::WavAudioFormat wavFormat;                     // The .wav file format
    std::unique_ptr<juce::AudioFormatWriter> writer;    // Writer object to write new processed sample to audio wav file


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectCodeAudioProcessor)
};
