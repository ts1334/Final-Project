/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ProjectCodeAudioProcessor::ProjectCodeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // Reference
    formatManager.registerBasicFormats();
    for (int i = 0; i < numVoices; i++)
    {
        sampler.addVoice(new juce::SamplerVoice());
    }
}

ProjectCodeAudioProcessor::~ProjectCodeAudioProcessor()
{
    // Reference
    formatReader = nullptr;
}

//==============================================================================
const juce::String ProjectCodeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ProjectCodeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ProjectCodeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ProjectCodeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ProjectCodeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ProjectCodeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ProjectCodeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ProjectCodeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ProjectCodeAudioProcessor::getProgramName (int index)
{
    return {};
}

void ProjectCodeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ProjectCodeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    // Reference
    sampler.setCurrentPlaybackSampleRate(sampleRate);
}

void ProjectCodeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ProjectCodeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ProjectCodeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }

    // Reference (from sampler vid)
    sampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool ProjectCodeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ProjectCodeAudioProcessor::createEditor()
{
    return new ProjectCodeAudioProcessorEditor (*this);
}

//==============================================================================
void ProjectCodeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    // Reference (EQ vid)
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void ProjectCodeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

// Reference (from loadFile from Sampler vid)
void ProjectCodeAudioProcessor::loadSample()
{
    sampler.clearSounds();

    juce::FileChooser fileChooser("Please load a file");

    if (fileChooser.browseForFileToOpen())
    {
        auto file = fileChooser.getResult();
        formatReader = formatManager.createReaderFor(file);
    }

    juce::BigInteger range;
    range.setRange(12, 128, true);

    auto sample = new juce::SamplerSound("Sample", *formatReader, range, 60, 0.1, 0.1, 10);

    auto sampleData = sample->getAudioData();

    int desiredBitDepth = 1;

    // Bitcrush file
    auto bitCrushedSampleData = bitCrushSample(sampleData, desiredBitDepth);

    // Write sound to a file
    // 
    // Create the file to write to DONE
    // Create FileOutputStream linked to this file 
    // Create the wavFormat that will be used to create a writer 
    // Create the writer that writes to this file
    // Write to the file from the buffer
    // 
    // Create Reader for the new file
    // Get a Sampler Sound from this Reader
    // 
    auto wavFormat = new juce::WavAudioFormat();
    auto bitCrushedWavFile = juce::File(juce::File::getCurrentWorkingDirectory().getFullPathName());
    auto outputStream(bitCrushedWavFile.createOutputStream());

    auto wavWriter = wavFormat->createWriterFor(outputStream.get(), 44100.0, 1, 16, juce::StringPairArray(), 0);

    wavWriter->writeFromAudioSampleBuffer(*bitCrushedSampleData, 0, bitCrushedSampleData->getNumSamples());
    wavWriter->~AudioFormatWriter();
    // Whack this into the sampler
    formatReader = formatManager.createReaderFor(bitCrushedWavFile);

    sampler.addSound(new juce::SamplerSound("BitCrushedSample", *formatReader, range, 60, 0.1, 0.1, 10));
}

// Reference (from loadFile)
void ProjectCodeAudioProcessor::loadSample(const juce::String& path)
{
    sampler.clearSounds();

    // Get the sound data
    auto file = juce::File(path);
    formatReader = formatManager.createReaderFor(file);

    juce::BigInteger range;
    range.setRange(12, 128, true);

    auto sample = new juce::SamplerSound("Sample", *formatReader, range, 60, 0.1, 0.1, 10);
    auto sampleData = sample->getAudioData();

    int desiredBitDepth = 1;

    // Bitcrush file
    auto bitCrushedSampleData = bitCrushSample(sampleData, desiredBitDepth);

    // Write sound to a file
    // 
    // Create the file to write to DONE
    // Create FileOutputStream linked to this file 
    // Create the wavFormat that will be used to create a writer 
    // Create the writer that writes to this file
    // Write to the file from the buffer
    // 
    // Create Reader for the new file
    // Get a Sampler Sound from this Reader
    // 
    auto wavFormat = new juce::WavAudioFormat();
    auto bitCrushedWavFile = juce::File(juce::File::getCurrentWorkingDirectory().getFullPathName());
    auto outputStream (bitCrushedWavFile.createOutputStream());

    auto wavWriter = wavFormat->createWriterFor(outputStream.get(), 44100.0, 1, 16, juce::StringPairArray(), 0);

    wavWriter->writeFromAudioSampleBuffer(*bitCrushedSampleData, 0, bitCrushedSampleData->getNumSamples());
    wavWriter->~AudioFormatWriter();

    // Whack this into the sampler
    formatReader = formatManager.createReaderFor(bitCrushedWavFile);

    sampler.addSound(new juce::SamplerSound("BitCrushedSample", *formatReader, range, 60, 0.1, 0.1, 10));
}

juce::AudioBuffer<float>* ProjectCodeAudioProcessor::convertSampleBitDepth(juce::AudioBuffer<float>* sampleData, int desiredBitDepth)
{
    int numSamples = sampleData->getNumSamples();

    auto magnitudeRange = sampleData->findMinMax(0, 0, numSamples);
    float minVal = magnitudeRange.getStart();
    float maxVal = magnitudeRange.getEnd();

    juce::Array<float> magnitudeValues = {};
    int numMagnitudeValues = 2 ^ desiredBitDepth;

    for (int i = 0; i < numMagnitudeValues; i++)
    {

        magnitudeValues.add(minVal + (maxVal-minVal)*(i/(numMagnitudeValues-1)));
    }

    for (int i = 0; i < numSamples; i++)
    {
        float currSample = sampleData->getSample(0, i);
        
        float currMagDif;
        float magToDesiredMagDif = INFINITY;
        float desiredMagnitude = INFINITY;
        for (int j = 0; j < numMagnitudeValues; j++)
        {
            currMagDif = abs(currSample - magnitudeValues[j]);
            if (currMagDif < magToDesiredMagDif)
            {
                magToDesiredMagDif = currMagDif;
                desiredMagnitude = magnitudeValues[j];
            }

        }
        sampleData->setSample(0, i, desiredMagnitude);
    }
    return sampleData;
}

juce::AudioBuffer<float>* ProjectCodeAudioProcessor::convertSampleSampleRate(juce::AudioBuffer<float>* sampleData)
{
    return sampleData;
}

juce::AudioBuffer<float>* ProjectCodeAudioProcessor::bitCrushSample(juce::AudioBuffer<float>* sampleData, int desiredBitDepth)
{
    auto bitDepthConvertedSampleData = convertSampleBitDepth(sampleData, desiredBitDepth);
    return bitDepthConvertedSampleData;
}

// Function to add a sample to the sampler
void ProjectCodeAudioProcessor::addSample(juce::SamplerSound sample)
{
    sampler.clearSounds();
    sampler.addSound(sample);
}

// Reference
juce::AudioProcessorValueTreeState::ParameterLayout
ProjectCodeAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout; layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("BitDepth", "BitDepth", juce::NormalisableRange<float>(1.f, 16.f, 1.f, 1.f), 4.f));
    // layout.add(std::make_unique<juce::AudioParameterFloat>("SampleRate", "SampleRate", juce::NormalisableRange<float>(40.f, 60000.f, 0.1f, 1.f), 22050.f));
    // layout.add(std::make_unique<juce::AudioParameterFloat>("SampleMidiNote", "SampleMidiNote", juce::NormalisableRange<float>(12.f, 128.f, 1.f, 1.f), 60.f));

    return layout;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProjectCodeAudioProcessor();
}
