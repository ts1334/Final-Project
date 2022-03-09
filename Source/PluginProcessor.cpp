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
    getAndSetParams();
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

    getAndSetParams();

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

    // Reference EQ vid
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
        getAndSetParams();
        if (sampleFile.exists())
        {
            updateSample(range);
        }
    }
}

void ProjectCodeAudioProcessor::getAndSetParams()
{
    params.sampleMIDINote = apvts.getRawParameterValue("SampleMidiNote")->load();
    params.bitDepth = apvts.getRawParameterValue("NESBitDepth")->load();
    int sampleRateIndex = apvts.getRawParameterValue("NESSampleRate")->load();
    switch (sampleRateIndex)
    {
        case 0:
            params.sampleRate = 4177.4;
            break;
        case 1:
            params.sampleRate = 4696.63;
            break;
        case 2:
            params.sampleRate = 5261.41;
            break;
        case 3:
            params.sampleRate = 5579.22;
            break;
        case 4:
            params.sampleRate = 6023.94;
            break;
        case 5:
            params.sampleRate = 7044.94;
            break;
        case 6:
            params.sampleRate = 7917.18;
            break;
        case 7:
            params.sampleRate = 8397.01;
            break;
        case 8:
            params.sampleRate = 9446.63;
            break;
        case 9:
            params.sampleRate = 11233.8;
            break;
        case 10:
            params.sampleRate = 12595.5;
            break;
        case 11:
            params.sampleRate = 14089.9;
            break;
        case 12:
            params.sampleRate = 16965.4;
            break;
        case 13:
            params.sampleRate = 21315.5;
            break;
        case 14:
            params.sampleRate = 25191.0;
            break;
        case 15:
            params.sampleRate = 33252.1;
            break;
    }

}

juce::BigInteger ProjectCodeAudioProcessor::getRange()
{
    return range;
}

// Reference (from loadFile from Sampler vid)
void ProjectCodeAudioProcessor::loadSample()
{
    sampler.clearSounds();

    juce::FileChooser fileChooser("Please load a file");
    if (fileChooser.browseForFileToOpen())
    {
        sampleFile = fileChooser.getResult();
        formatReader = formatManager.createReaderFor(sampleFile);

        range.setRange(12, 128, true);

        auto sample = new juce::SamplerSound("Sample", *formatReader, range, 60, 0, 0, 10);

        // get sampleData
        sampleData = sample->getAudioData();

        updateSample(range);
    }
}

// Reference (from loadFile)
void ProjectCodeAudioProcessor::loadSample(const juce::String& path)
{
    sampler.clearSounds();

    range.setRange(12, 128, true);

    // Get the sound data
    sampleFile = juce::File(path);
    formatReader = formatManager.createReaderFor(sampleFile);

    auto sample = new juce::SamplerSound("Sample", *formatReader, range, 60, 0, 0, 10);

    // get sampleData
    sampleData = sample->getAudioData();

    updateSample(range);
}

bool ProjectCodeAudioProcessor::sampleLoaded()
{
    if (sampleFile.exists())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ProjectCodeAudioProcessor::updateSample(juce::BigInteger range)
{
    // Bitcrush the audio data file,
    DBG(params.bitDepth);
    DBG(params.sampleRate);
    bitCrushSample(sampleData, params.sampleRate, params.bitDepth, true, 1);

    juce::File bitCrushedWavFile = juce::File(sampleFile.getParentDirectory().getChildFile("bitCrushed.wav"));
    auto outputStream = new juce::FileOutputStream(bitCrushedWavFile);
    writer.reset(wavFormat.createWriterFor(outputStream, 44100.0, 1, 16, {}, 0));

    if (writer != nullptr)
    {
        writer->writeFromAudioSampleBuffer(*sampleData, 0, sampleData->getNumSamples());
    }
    writer->~AudioFormatWriter();

    // Whack this into the sampler
    formatReader = formatManager.createReaderFor(bitCrushedWavFile);

    sampler.addSound(new juce::SamplerSound("BitCrushedSample", *formatReader, range, params.sampleMIDINote, 0, 0, 10));

    bitCrushedWavFile.deleteFile();
}

void ProjectCodeAudioProcessor::bitCrushSample(juce::AudioBuffer<float>* sampleData, float desiredSampleRate, int desiredBitDepth, bool DPCM, int DPCMDepth = 0)
{
    convertSampleSampleRate(sampleData, desiredSampleRate);
    if (DPCM)
    {
        convertSampleBitDepthDPCM(sampleData, desiredSampleRate, desiredBitDepth, DPCMDepth);
    }
    else
    {
        convertSampleBitDepthPCM(sampleData, desiredBitDepth);
    }

}

void ProjectCodeAudioProcessor::convertSampleSampleRate(juce::AudioBuffer<float>* sampleData, float desiredSampleRate)
{
    int numSamples = sampleData->getNumSamples();

    float increment = getSampleRate() / desiredSampleRate;

    float currPos = 0;

    float currSample;
    // Assume a straight line between samples for simplicity's sake
    while (currPos <= numSamples-1)
    {
        //DBG(currPos);
        // If curr pos is a whole number just equate the samples
        if (floor(currPos) == currPos)
        {
            currSample = sampleData->getSample(0, currPos);
        }

        // If not then find the value it would be between the samples in question
        else
        {
            float previousSample = sampleData->getSample(0, (int)floor(currPos));
            float followingSample = sampleData->getSample(0, (int)ceil(currPos));
            currSample = previousSample + (currPos - floor(currPos)) * (followingSample - previousSample);
        }

        // Fill samples between current and the one up to next increment with calculated value
        if (currPos + increment > numSamples-1)
        {
            for (int i = ceil(currPos); i < numSamples; i++)
            {
                sampleData->setSample(0, i, currSample);
            }
        }

        else
        {
            for (int i = ceil(currPos); i < currPos + increment; i++)
            {
                sampleData->setSample(0, i, currSample);
            }
        }

        currPos += increment;
    }
}

void ProjectCodeAudioProcessor::convertSampleBitDepthDPCM(juce::AudioBuffer<float>* sampleData, float sampleRateConverted, int desiredBitDepth, int slopeBitDepth)
{
    int numSamples = sampleData->getNumSamples();
    auto sampleValRange = sampleData->findMinMax(0, 0, numSamples);
    auto sampleMaxVal = sampleValRange.getEnd();    // This will correspond to 1
    auto sampleMinVal = sampleValRange.getStart();  // This will correspond to -1

    if (abs(sampleMaxVal) > abs(sampleMinVal))
    {
        sampleData->applyGain(1 / abs(sampleMaxVal));
    }

    else
    {
        sampleData->applyGain(1 / abs(sampleMinVal));
    }

    juce::Array<float> magnitudeValues = {};
    int numMagnitudeValues = pow(2, desiredBitDepth);
    float maxVal = 1;
    float minVal = -1 + 1 / (0.5 * numMagnitudeValues);

    float magIncrement = (maxVal - minVal) / (numMagnitudeValues - 1);

    int numSlopeVals = pow(2, slopeBitDepth);

    // Calculate possible changes in magnitude values (i.e. -1 mag value or +2 mag values)
    juce::Array<int> possibleChangesInMagVals = {};
    for (int i = -numSlopeVals / 2; i < numSlopeVals / 2; i++)
    {
        possibleChangesInMagVals.add(i);
    }

    int calcBufferSize = numSamples * (sampleRateConverted / getSampleRate());
    auto calcBuffer = new juce::AudioSampleBuffer(1, calcBufferSize);
    float currSample = 0;
    calcBuffer->setSample(0, 0, currSample);

    float increment = getSampleRate() / sampleRateConverted;

    for (int i = 1; i < calcBufferSize; i++)
    {
        float nextSample = sampleData->getSample(0, ceil(i * increment));

        float currMagDif;
        float desiredMagDif = INFINITY;
        float desiredMagnitude = 0;
        // Check which possible increase or decrease brings value closest to actual one
        for (int change : possibleChangesInMagVals)
        {
            currMagDif = abs(nextSample - (currSample + change * magIncrement));
            if (currMagDif < desiredMagDif)
            {
                desiredMagnitude = currSample + change * magIncrement;
            }
        }

        calcBuffer->setSample(0, i, desiredMagnitude);

        currSample = nextSample;
    }

    // Finally, assign calculated values to the main sampleData buffer

    int currPos = 0;
    for (int i = 0; i < calcBufferSize; i++)
    {
        for (int j = currPos; j < currPos + increment; j++)
        {
            sampleData->setSample(0, j, calcBuffer->getSample(0, i));
        }
        currPos = ceil(currPos + increment);
    }
}

// Convert bit depth like PCM and also scale up amplitude to 1 (-1 to 1)
void ProjectCodeAudioProcessor::convertSampleBitDepthPCM(juce::AudioBuffer<float>* sampleData, int desiredBitDepth)
{
    int numSamples = sampleData->getNumSamples();
    auto sampleValRange = sampleData->findMinMax(0, 0, numSamples);
    auto sampleMaxVal = sampleValRange.getEnd();    // This will correspond to 1
    auto sampleMinVal = sampleValRange.getStart();  // This will correspond to -1

    if (abs(sampleMaxVal) > abs(sampleMinVal))
    {
        sampleData->applyGain(1/ abs(sampleMaxVal));
    }

    else
    {
        sampleData->applyGain(1 / abs(sampleMinVal));
    }

    juce::Array<float> magnitudeValues = {};
    float numMagnitudeValues = pow(2, desiredBitDepth);
    float maxVal = 1;
    float minVal = -1 + 1 / (0.5 * numMagnitudeValues);
    // I want it to be set so the middle sample is 0 (so for bit depth of 7 bit 63 should be 0)
    for (int i = 0; i < numMagnitudeValues; i++)
    {
        magnitudeValues.add(minVal + (maxVal - minVal) * (i / (numMagnitudeValues - 1)));
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

    layout.add(std::make_unique<juce::AudioParameterChoice>("Console", "Console", juce::StringArray("NES", "SNES", "GameBoy", "GBA"), 0));
    layout.add(std::make_unique<juce::AudioParameterInt>("SampleMidiNote", "SampleMidiNote", 12, 128, 60));
    layout.add(std::make_unique<juce::AudioParameterInt>("NESBitDepth", "NESBitDepth", 1, 7, 7));
    layout.add(std::make_unique<juce::AudioParameterChoice>("NESSampleRate", "NESSampleRate", juce::StringArray("4177.4", "4696.63", "5261.41", "5579.22", "6023.94", "7044.94", "7917.18", "8397.01", "9446.63", "11233.8", "12595.5", "14089.9", "16965.4", "21315.5", "25191.0", "33252.1"), 0));

    return layout;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProjectCodeAudioProcessor();
}
