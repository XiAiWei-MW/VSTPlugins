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
class PingPongDelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PingPongDelayAudioProcessor();
    ~PingPongDelayAudioProcessor() override;

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

    //==============================================================================
    float gain{ 1.0 };
    float delayTime{ 1000.0 };
    float feedback{ 0.6 };
    float wetDry{ 0.9 };
    float width{ 0.5 };
    float LFOTime{ 0.7 };

    int currRead;
    int currWrite;
    const int maxSample = 2.0 * 44100;
    int delaySample = delayTime / 1000 * 44100;

    //==============================================================================
    juce::AudioBuffer<float> delayBuffer;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLine{ maxSample };

private:
    //==============================================================================
    double lastSampleRate;
    int currPoint = 0;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PingPongDelayAudioProcessor)
};
