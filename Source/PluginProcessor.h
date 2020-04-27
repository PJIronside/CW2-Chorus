/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class ChorusAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    ChorusAudioProcessor();
    ~ChorusAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusAudioProcessor)

        AudioParameterFloat* mixA;
        AudioParameterFloat* mixB;
        AudioParameterFloat* widthA;
        AudioParameterFloat* widthB;
        AudioParameterFloat* speedA;
        AudioParameterFloat* speedB;
        AudioParameterFloat* timeA;
        AudioParameterFloat* timeB;

        AudioBuffer<float> delayBufferA;
        AudioBuffer<float> delayBufferB;
        int delayBufferALength = 1;
        int delayBufferBLength = 1;
        int readIndexA = 0;
        int readIndexB = 0;
        int writeIndexA = 0;
        int writeIndexB = 0;

      
};
