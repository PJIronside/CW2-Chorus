/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChorusAudioProcessor::ChorusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    mixA = new AudioParameterFloat("mixA", "Mix A", 0.0f, 1.0f, 0.0f);
    addParameter(mixA);
    mixB = new AudioParameterFloat("mixB", "Mix B", 0.0f, 1.0f, 0.0f);
    addParameter(mixB);
    intensityA = new AudioParameterFloat("intensityA", "Intensity A", 0.0000f, 0.0200f, 0.0100f);
    addParameter(intensityA);
    intensityB = new AudioParameterFloat("intensityB", "Intensity B", 0.0000f, 0.0200f, 0.0100f);
    addParameter(intensityB);
    speedA = new AudioParameterFloat("speedA", "Speed A", 0.100f, 3.000f, 1.000f);
    addParameter(speedA);
    speedB = new AudioParameterFloat("speedB", "Speed B", 0.100f, 3.000f, 1.000f);
    addParameter(speedB);
}

ChorusAudioProcessor::~ChorusAudioProcessor()
{
}

//==============================================================================
const String ChorusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChorusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChorusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChorusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChorusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChorusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ChorusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChorusAudioProcessor::setCurrentProgram (int index)
{
}

const String ChorusAudioProcessor::getProgramName (int index)
{
    return {};
}

void ChorusAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ChorusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    // Maximum delay of 1 seconds
    delayBufferLength = (int)(sampleRate);
    // Set the buffer to 1 channel of the size of delayBufferLength using setSize
    delayBufferA.setSize(1, delayBufferLength);
    delayBufferB.setSize(1, delayBufferLength);
    // Set all the samples in the buffer to zero
    delayBufferA.clear();
    delayBufferB.clear();
   
}

void ChorusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChorusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void ChorusAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
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

    // IMPORTANT: calculate the position of the read index relative to the write index
// i.e. the delay time in samples

    readIndexA = (int)(writeIndexA - (intensityA->get() * getSampleRate()) * +delayBufferLength) % delayBufferLength;
    readIndexB = (int)(writeIndexB - (intensityB->get() * getSampleRate()) * +delayBufferLength) % delayBufferLength;
       

        // channelData is an array of length numSamples which contain
        // the audio for one channel
        float *channelData = buffer.getWritePointer(0);
        // delayData is the circular buffer for the delay
        float *delayAData = delayBufferA.getWritePointer(0);
        float *delayBData = delayBufferB.getWritePointer(0);

        float intensityAValue = intensityA->get();
        float intensityBValue = intensityB->get();
        float speedAValue = speedA->get();
        float speedBValue = speedB->get();
        float mixAValue = mixA->get();
        float mixBValue = mixB->get();

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            // Retrieve the input sample and store locally
            auto in = channelData[i];
            auto modulationA = 0.02 + (intensityAValue * 0.5) + sin(2 * 3.141 * speedAValue * channelData[i]);
            auto modulationB = 0.02 + (intensityBValue * 0.5) + sin(2 * 3.141 * speedBValue * channelData[i]);
            
            //Compute the output sample
            auto outLeft = in + (mixAValue * (modulationA - delayAData[readIndexA]));
            auto outRight = in + (mixBValue * (modulationB - delayBData[readIndexB]));

            // Write the current input into the delay buffer along with the delayed sample
            delayAData[writeIndexA] = in + delayAData[readIndexA];
            delayBData[writeIndexB] = in + delayBData[readIndexB];

            // Increment read index then check to see if it's greater than the buffer length
            // If so wrap back around to zero
            if (++readIndexA >= delayBufferLength)
                readIndexA = 0;
            if (++readIndexB >= delayBufferLength)
                readIndexB = 0;
            // Same with write index
            if (++writeIndexA >= delayBufferLength)
                writeIndexA = 0;
            if (++writeIndexB >= delayBufferLength)
                writeIndexB = 0;
            
            channelData[i] = in + outLeft + outRight;
        }
    
}

//==============================================================================
bool ChorusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ChorusAudioProcessor::createEditor()
{
    return new GenericAudioProcessorEditor (*this);
}

//==============================================================================
void ChorusAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ChorusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChorusAudioProcessor();
}
