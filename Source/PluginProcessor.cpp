/*
  ==============================================================================

    This file is adapted from the basic framework code for a JUCE plugin processor.
    Sections beginning with //================== are generated, adapted, added to
    or edited from this original auto generated code

    Implementation file for the processor of a JUCE VST video game sample emulation plugin
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
    // From [2], set to register basic formats!
    formatManager.registerBasicFormats();

    // From [2]. Adds the correct number of voices to the sampler
    for (int i = 0; i < numVoices; i++)
    {
        sampler.addVoice(new juce::SamplerVoice());
    }
}

ProjectCodeAudioProcessor::~ProjectCodeAudioProcessor()
{
    // From [2]. Destroy formatReader object upon closing to ensure no leaking
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
    
    // From [2] initialise sampler's sample rate and parameters before playback 
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

    getAndSetParams();  // Update parameters as program goes

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

        // In this plugin's case, the data is processed separately, as processing
        // takes place on only the initial sample data, not the output of the plugin
    }

    // From [2]
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

    // From [1]
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void ProjectCodeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // Adapted from [1]
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
        getAndSetParams();          // Update parameters with the current controller values
        if (sampleFile.exists())
        {
            updateSample(range);
        }
    }
}

// Function to store the current values of the parameter controls in the parameter object
void ProjectCodeAudioProcessor::getAndSetParams()
{
    int consoleIndex = apvts.getRawParameterValue("Console")->load();   // Get the index of the current Console selected
    int DPCMIndex;  // Initialised to store index of parameter to choose whether DPCM or PCM is to be used

    // Check the console index
    switch (consoleIndex)
    {
        // If '0' then the console selected is NES
        case 0:
            params.console = "NES"; // Store that current console is NES
            DPCMIndex = apvts.getRawParameterValue("PCMorDPCM")->load();    // Get the index of whether PCM or DPCM is selected
            // If 0, PCM is selected
            if (DPCMIndex == 0)
            {
                params.DPCM = false;    // Set DPCM to not be emulated, (so PCM will be used instead)
            }
            // If 1, DPCM is selected
            else if (DPCMIndex == 1)
            {
                params.DPCM = true;     // Set DPCM to be emulated
            }
            break;
        // If '1' then the console selected is SNES
        case 1:
            params.console = "SNES";    // Store that current console is SNES
            params.DPCM = true;         // Set DPCM to be emulated
            break;
        // If '2' then the console selected is GameBoy
        case 2:
            params.console = "GameBoy"; // Store that current console is GameBoy
            break;
        // If '3' then the console selected is GBA
        case 3:
            params.console = "GBA"; // Store that current console is GBA
            break;
    }
    
    params.sampleMIDINote = apvts.getRawParameterValue("SampleMidiNote")->load();   // Store the currently selected starting MIDI Note

    // Check if the currently selected console is NES
    if (params.console == "NES")
    {
        params.bitDepth = apvts.getRawParameterValue("NESBitDepth")->load();        // Set desired bit depth to the current value of the NES bit depth slider
        params.DPCMBit = 1;                                                         // Set DPCM parameter to 1 bit
        int sampleRateIndex = apvts.getRawParameterValue("NESSampleRate")->load();  // Get the currently selected index of the NES sample rate slider

        // Set desired sample rate to appropriate value according to index
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
    
    // If not, check if the currently selected console is SNES
    else if (params.console == "SNES")
    {
        params.bitDepth = apvts.getRawParameterValue("SNESBitDepth")->load();       // Set desired bit depth to the current value of the SNES bit depth slider
        params.DPCMBit = apvts.getRawParameterValue("SNESDPCMBit")->load();         // Set desired DPCM bit number to the current value of the SNES DPCM bit slider
        int sampleRateIndex = apvts.getRawParameterValue("SNESSampleRate")->load(); // Get the currently selected index of the SNES sample rate slider
        
        // For now just set sample rate to 32kHz for either index. To be changed after further research
        switch (sampleRateIndex)
        {
        case 0:
            params.sampleRate = 32000.0;
            break;
        case 1:
            params.sampleRate = 32000.0;
            break;
        }
    }

    // If not, check if the currently selected console is GameBoy
    else if (params.console == "GameBoy")
    {

    }

    // If not, check if the currently selected console is GBA
    else if (params.console == "GBA")
    {

    }
}

juce::BigInteger ProjectCodeAudioProcessor::getRange()
{
    return range;
}

// Adapted from [2] Loads an audio sample's data, selected in file browser
void ProjectCodeAudioProcessor::loadSample()
{
    juce::FileChooser fileChooser("Please load a file");

    // If browsing for a file (so button is pressed)
    if (fileChooser.browseForFileToOpen())
    {
        sampleFile = fileChooser.getResult();                       // Get the file selected
        formatReader = formatManager.createReaderFor(sampleFile);   // Create a reader for this file

        range.setRange(12, 128, true);  // Set range of MIDI notes

        auto sample = new juce::SamplerSound("Sample", *formatReader, range, params.sampleMIDINote, 0, 0, 10);  // Create a new SamplerSound object from this file using the reader

        originalSampleData = sample->getAudioData();    // Get the data associated with this sound 

        updateSample(range);    // Update VST's sample by processing the original data
    }
}

// Adapted from [2] Loads an audio sample's data, dragged and dropped from file explorer
void ProjectCodeAudioProcessor::loadSample(const juce::String& path)
{
    range.setRange(12, 128, true);  // Set range of MIDI notes

    
    sampleFile = juce::File(path);                              // Get the dropped file as an object
    formatReader = formatManager.createReaderFor(sampleFile);   // Create a reader for this file

    auto sample = new juce::SamplerSound("Sample", *formatReader, range, params.sampleMIDINote, 0, 0, 10);  // Create a new SamplerSound object from this file using the reader

    originalSampleData = sample->getAudioData();    // Get the data associated with this sound

    updateSample(range);    // Update VST's sample by processing the original data
}

// Function to check if a sample is loaded and return result (true or false)
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

// Updates the VST's current sample to a new updated one
void ProjectCodeAudioProcessor::updateSample(juce::BigInteger range)
{
    sampler.clearSounds();  // Remove any sounds stored in the VST

    auto processedSampleData = *originalSampleData;                                                         // Copy the original sample data as a new object
    bitCrushSample(&processedSampleData, params.sampleRate, params.bitDepth, params.DPCM, params.DPCMBit);  // Process the copy of the audio data

    juce::File bitCrushedWavFile = juce::File(sampleFile.getParentDirectory().getChildFile("bitCrushed.wav"));  // Create a new/get the file in the same directory as the original sample file to store the processed audio data
    auto outputStream = new juce::FileOutputStream(bitCrushedWavFile);  // Create an output stream to allow the data to be output to the file

    // Check the output stream is valid
    if (outputStream->openedOk())
    {
        // Remove any residual data inside the audio file
        outputStream->setPosition(0);
        outputStream->truncate();
        
        writer.reset(wavFormat.createWriterFor(outputStream, 44100.0, 1, 16, {}, 0));   // Create a new audio format writer (overwriting any previous) to write to the output stream 

        // If the writer is valid/exists
        if (writer != nullptr)
        {
            writer->writeFromAudioSampleBuffer(processedSampleData, 0, processedSampleData.getNumSamples());    // Write the processed sample data to the output stream, and thus the file
        }

        outputStream->flush();  // Ensure all data is written
        writer = nullptr;       // Destroy writer

        formatReader = formatManager.createReaderFor(bitCrushedWavFile);                                                        // Read new audio file
        sampler.addSound(new juce::SamplerSound("BitCrushedSample", *formatReader, range, params.sampleMIDINote, 0, 0, 10));    // Add to sampler
    }
}

// Higher level bit crush function for processing the sample data
void ProjectCodeAudioProcessor::bitCrushSample(juce::AudioBuffer<float>* sampleData, float desiredSampleRate, int desiredBitDepth, bool DPCM, int DPCMDepth)
{
    convertSampleSampleRate(sampleData, desiredSampleRate); // Convert the sample rate of the given data to the specified value
    // Check whether sampling method to emulate is DPCM
    if (DPCM)
    {
        convertSampleBitDepthDPCM(sampleData, desiredSampleRate, desiredBitDepth, DPCMDepth);   // If DPCM, convert audio bit depth according to DPCM parameters
    }
    else
    {
        convertSampleBitDepthPCM(sampleData, desiredBitDepth);  // If not DPCM, convert audio bit depth according to PCM
    }
}

// Sample rate conversion function which effectively converts sample rate by locking sample values for a certain section to the first value in that setion
void ProjectCodeAudioProcessor::convertSampleSampleRate(juce::AudioBuffer<float>* sampleData, float desiredSampleRate)
{
    int numSamples = sampleData->getNumSamples();   // Get the number of samples in the data

    float increment = getSampleRate() / desiredSampleRate;  // Calculate the number of samples between each 'sample' (i.e. the size of each section in samples)

    float currPos = 0;  // Set the starting position to the start of the data (sample indexed 0)

    float currSample;   // Variable to register the value of the current sample (i.e. amplitude value)

    // While the current position remains within the size of the sample data
    while (currPos <= numSamples-1)
    {
        // If curr pos is a whole number just equate the samples
        if (floor(currPos) == currPos)
        {
            currSample = sampleData->getSample(0, currPos);
        }

        // If not then find the value it would be between the samples in question
        else
        {
            float previousSample = sampleData->getSample(0, (int)floor(currPos));   // Value of the previous actual sample
            float followingSample = sampleData->getSample(0, (int)ceil(currPos));   // Value of the following actual sample
            currSample = previousSample + (currPos - floor(currPos)) * (followingSample - previousSample);  // Assume a straight line between samples for now for simplicity's sake to calculate effective 'sample' value
        }
        
        // Fill samples between current and the one up to next increment with calculated value
        // If the current position plus the increment would exceed bounds of data, just go up to the final sample in the data
        if (currPos + increment > numSamples-1)
        {
            for (int i = ceil(currPos); i < numSamples; i++)
            {
                sampleData->setSample(0, i, currSample);
            }
        }

        // Else go right up to the next increment point
        else
        {
            for (int i = ceil(currPos); i < currPos + increment; i++)
            {
                sampleData->setSample(0, i, currSample);
            }
        }

        currPos += increment;   // Set new current position
    }
}

// Convert Bit Depth Using DPCM and scale max or min values to 1 or -1
void ProjectCodeAudioProcessor::convertSampleBitDepthDPCM(juce::AudioBuffer<float>* sampleData, float sampleRateConverted, int desiredBitDepth, int slopeBitDepth)
{
    int numSamples = sampleData->getNumSamples();                   // Get the number of samples in the data
    auto sampleValRange = sampleData->findMinMax(0, 0, numSamples); // Get the minimum and maximum values contained in the sample data
    auto sampleMaxVal = sampleValRange.getEnd();                    // Max
    auto sampleMinVal = sampleValRange.getStart();                  // Min

    sampleData->applyGain(1 / abs(sampleMaxVal));   // Make the absolute value of the maximum or minimum of the original data (whichever is bigger) equal to 1

    float numMagnitudeValues = pow(2, desiredBitDepth);   // Calculate number of discrete amplitude values (2 to the power of desired bit depth)
    float maxVal = 1;                                   // Maximum amplitude value set to 1
    float minVal = -1 + 1 / (0.5 * numMagnitudeValues); // Minimum set to -1 plus one amplitude value increment

    float magIncrement = (maxVal - minVal) / (numMagnitudeValues - 1);  // Get the amplitude value increment (i.e. the gap between values)
    
    int numSlopeVals = pow(2, slopeBitDepth);   // Number of possible amplitude increments that can occur from one sample to the next

    // Calculate possible changes in magnitude values (in number of magnitude steps/increments) (e.g. for DPCM bit of 4, this is -2 -1, +1 and +2)
    juce::Array<int> possibleChangesInMagVals = {};
    for (int i = -numSlopeVals / 2; i <= numSlopeVals / 2; i++)
    {
        if (i != 0)
        {
            possibleChangesInMagVals.add(i);
        }
    }

    int calcBufferSize = floor(numSamples * (sampleRateConverted / getSampleRate()));   // Get size of calculation buffer
    auto calcBuffer = new juce::AudioSampleBuffer(1, calcBufferSize);                   // Create a buffer which contains the each effective sample by sample value (to store calculated values)
    
    float currSample = 0;                       // Set the current sample's value to 0
    calcBuffer->setSample(0, 0, currSample);    // Set first sample in calculation buffer to 0

    float increment = getSampleRate() / sampleRateConverted;    // Get the sample increment used for sample rate conversion

    for (int i = 1; i < calcBufferSize; i++)
    {
        float nextSample = sampleData->getSample(0, ceil(i * increment));

        float currMagDif;
        float lowestMagDif = INFINITY;  // Initialise to INFINITY as method works by cycling through and updating the desired converted value for the next sample by checking which is closest to the actual
        float desiredMagnitude = 0;
        // Check which possible increase or decrease brings value closest to actual one
        for (int change : possibleChangesInMagVals)
        {
            currMagDif = abs(nextSample - (currSample + change * magIncrement));
            if (currMagDif < lowestMagDif && currSample + change * magIncrement >= minVal && currSample + change * magIncrement <= maxVal)
            {
                lowestMagDif = currMagDif;
                desiredMagnitude = currSample + change * magIncrement;          
            }
        }
        
        calcBuffer->setSample(0, i, desiredMagnitude);  // Store the desired value of the current sample in the calculation buffer

        currSample = desiredMagnitude;  // Store the current sample's value
    }

    // Finally, assign calculated values to the main sampleData buffer
    float currPos = 0;
    for (int i = 0; i < calcBufferSize; i++)
    {
        for (int j = ceil(currPos); j < currPos + increment; j++)
        {
            if (j < sampleData->getNumSamples())
            {
                sampleData->setSample(0, j, calcBuffer->getSample(0, i));
            }
        }
        currPos += increment;
    }
}

// Convert bit depth like PCM by rounding sample values to nearest discrete value according to desired bit depth
void ProjectCodeAudioProcessor::convertSampleBitDepthPCM(juce::AudioBuffer<float>* sampleData, int desiredBitDepth)
{
    int numSamples = sampleData->getNumSamples();                   // Get the number of samples in the data
    auto sampleValRange = sampleData->findMinMax(0, 0, numSamples); // Get the minimum and maximum values contained in the sample data
    auto sampleMaxVal = sampleValRange.getEnd();                    // Max
    auto sampleMinVal = sampleValRange.getStart();                  // Min

    sampleData->applyGain(1 / abs(sampleMaxVal));

    juce::Array<float> magnitudeValues = {};
    float numMagnitudeValues = pow(2, desiredBitDepth);   // Calculate number of discrete amplitude values (2 to the power of desired bit depth)
    float maxVal = 1;                                   // Maximum amplitude value set to 1
    float minVal = -1 + 1 / (0.5 * numMagnitudeValues); // Minimum set to -1 plus one amplitude value increment

    // Store the possible discrete magnitude values
    // I want it to be set so the middle sample is 0 (so for bit depth of 7 bit 63 should be 0)
    for (int i = 0; i < numMagnitudeValues; i++)
    {
        magnitudeValues.add(minVal + (maxVal - minVal) * (i / (numMagnitudeValues - 1)));
    }

    // Cycle through each sample in the data
    for (int i = 0; i < numSamples; i++)
    {
        float currSample = sampleData->getSample(0, i); // Store the current sample's original value

        float currMagDif;   
        float magToDesiredMagDif = INFINITY;    // Initialise high as programme works by checking which is lowest
        float desiredMagnitude = INFINITY;      // Initialise

        // Cycle through each possible magnitude value and find the one which is closest to the original value
        for (int j = 0; j < numMagnitudeValues; j++)
        {
            currMagDif = abs(currSample - magnitudeValues[j]);
            if (currMagDif < magToDesiredMagDif)
            {
                magToDesiredMagDif = currMagDif;
                desiredMagnitude = magnitudeValues[j];
            }

        }
        sampleData->setSample(0, i, desiredMagnitude);  // Set the sample to the new magnitude value (the new discrete one closest to the original)
    }
}

// Adapted from [1] Create the audio parameter layout
juce::AudioProcessorValueTreeState::ParameterLayout
ProjectCodeAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout; layout;

    // General parameters
    layout.add(std::make_unique<juce::AudioParameterChoice>("Console", "Console", juce::StringArray("NES", "SNES", "GameBoy", "GBA"), 0));  // Console selection parameter
    layout.add(std::make_unique<juce::AudioParameterInt>("SampleMidiNote", "SampleMidiNote", 12, 128, 60));                                 // MIDI note of original sample parameter
    layout.add(std::make_unique<juce::AudioParameterChoice>("PCMorDPCM", "PCMorDPCM", juce::StringArray("PCM", "DPCM"), 0));                // PCM or DPCM selection parameter

    // NES parameters
    layout.add(std::make_unique<juce::AudioParameterInt>("NESBitDepth", "NESBitDepth", 1, 7, 7));                                           // NES bit depth parameter
    layout.add(std::make_unique<juce::AudioParameterChoice>("NESSampleRate", "NESSampleRate", juce::StringArray("4177.4", "4696.63",        // NES sample rate parameter
                                                                                                "5261.41", "5579.22", "6023.94", "7044.94", 
                                                                                                "7917.18", "8397.01", "9446.63", "11233.8", 
                                                                                                "12595.5", "14089.9", "16965.4", "21315.5", "25191.0", "33252.1"), 0));    
    // SNES parameters        
    layout.add(std::make_unique<juce::AudioParameterInt>("SNESBitDepth", "SNESBitDepth", 1, 15, 15));                                           // SNES bit depth parameter
    layout.add(std::make_unique<juce::AudioParameterChoice>("SNESSampleRate", "SNESSampleRate", juce::StringArray("32000.0", "32000.0"), 0));   // SNES sample rate parameter
    layout.add(std::make_unique<juce::AudioParameterInt>("SNESDPCMBit", "SNESDPCMBit", 1, 4, 4));                                               // SNES DPCM bits parameter

    return layout;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProjectCodeAudioProcessor();
}
