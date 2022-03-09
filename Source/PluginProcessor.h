/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct Parameters
{
    int sampleMIDINote = 60;
    int bitDepth = 16;
    float sampleRate = 44100;
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

    void getAndSetParams();

    juce::BigInteger getRange();

    // Reference
    void loadSample();
    void loadSample(const juce::String& path);

    bool sampleLoaded();

    void updateSample(juce::BigInteger range);

    // Bit depth conversion
    void convertSampleBitDepthDPCM(juce::AudioBuffer<float>* sampleData, float sampleRateConverted, int desiredBitDepth, int slopeBitDepth);
    void convertSampleBitDepthPCM(juce::AudioBuffer<float>* sampleData, int desiredBitDepth);

    // Sample rate conversion
    void convertSampleSampleRate(juce::AudioBuffer<float>* sampleData, float desiredSampleRate);

    // Overall bit crushing
    void bitCrushSample(juce::AudioBuffer<float>* sampleData, float desiredSampleRate, int desiredBitDepth, bool DPCM, int DPCMDepth = 0);
    void addSample(juce::SamplerSound sample);

    int getNumSamplerSounds() { return sampler.getNumSounds(); }

    // Reference
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    // Reference
    juce::Synthesiser sampler;
    juce::AudioSampleBuffer* sampleData;
    juce::File sampleFile;
    juce::BigInteger range;
    const int numVoices{ 1 };

    Parameters params;

    juce::AudioFormatManager formatManager;
    juce::AudioFormatReader* formatReader{ nullptr };

    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectCodeAudioProcessor)
};
