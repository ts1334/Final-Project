/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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

    // Reference
    void loadSample();
    void loadSample(const juce::String& path);


    juce::AudioBuffer<float>* ProjectCodeAudioProcessor::convertSampleBitDepth(juce::AudioBuffer<float>* sampleData, int desiredBitDepth);
    juce::AudioBuffer<float>* ProjectCodeAudioProcessor::convertSampleSampleRate(juce::AudioBuffer<float>* sampleData);
    juce::AudioBuffer<float>* ProjectCodeAudioProcessor::bitCrushSample(juce::AudioBuffer<float>* sampleData, int desiredBitDepth);
    void addSample(juce::SamplerSound sample);

    int getNumSamplerSounds() { return sampler.getNumSounds(); }

    // Reference
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    // Reference
    juce::Synthesiser sampler;
    const int numVoices{ 3 };

    juce::AudioFormatManager formatManager;
    juce::AudioFormatReader* formatReader{ nullptr };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectCodeAudioProcessor)
};
