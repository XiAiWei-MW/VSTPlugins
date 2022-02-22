/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PingPongDelayAudioProcessor::PingPongDelayAudioProcessor()
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
}

PingPongDelayAudioProcessor::~PingPongDelayAudioProcessor()
{
}

//==============================================================================
const juce::String PingPongDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PingPongDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PingPongDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PingPongDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PingPongDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PingPongDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PingPongDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PingPongDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PingPongDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void PingPongDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PingPongDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currRead = 0;
    /*lastSampleRate = sampleRate;
    delayBuffer.setSize(2, 2.0 * (sampleRate * samplesPerBlock));
    currWrite = fminf(currRead + delayTime, 2.0) * (sampleRate * samplesPerBlock);
    delayBuffer.clear();*/
    //maxSample = 2.0 * getSampleRate() * getBlockSize();

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    spec.sampleRate = sampleRate;

    delayLine.prepare(spec);
    delayLine.reset();
    delayLine.setDelay(delaySample);
}

void PingPongDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PingPongDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PingPongDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    /*juce::dsp::AudioBlock<float> block(buffer);
    auto* channelLeft = block.getChannelPointer(0);
    auto* channelRight = block.getChannelPointer(1);*/
    auto* channelLeft = buffer.getWritePointer(0);
    auto* channelRight = buffer.getWritePointer(1);

    const int bufferLength = buffer.getNumSamples();

    //juce::dsp::ProcessContextReplacing<float> context{ block };
    //delayLine.process(context);

    /*
    auto* channelLeft = buffer.getWritePointer(0);
    auto* channelRight = buffer.getWritePointer(1);
    auto* delayLeft = delayBuffer.getWritePointer(0);
    auto* delayRight = delayBuffer.getWritePointer(1);
    */

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (auto i = 0; i < bufferLength; ++i)
    {
        auto LFO = (int) (LFOTime * getSampleRate() / 1000);
        if (i % LFO == 0) { currPoint = 1 - currPoint; }

        float left = channelLeft[i];
        float right = channelRight[i];

        if (currPoint == 0)
        {
            float panning = (width + 1.0) / 2.0;
            channelLeft[i] = channelLeft[i] * (1.0 - panning);
            channelRight[i] = channelRight[i] * panning;
        }
        else
        {
            float panning = (1.0 - width) / 2.0;
            channelLeft[i] = channelLeft[i] * (1.0 - panning);
            channelRight[i] = channelRight[i] * panning;
        }

        auto currDelayLeft = delayLine.popSample(0, delaySample, true);
        auto currDelayRight = delayLine.popSample(1, delaySample, true);
        //DBG(currDelayLeft << " " << currDelayRight);

        float outputLeft = channelLeft[i] + wetDry * currDelayRight;
        float outputRight = channelRight[i] + wetDry * currDelayLeft;
        //DBG(outputLeft << " " << outputRight);

        delayLine.pushSample(0, channelLeft[i] + currDelayRight * feedback);
        delayLine.pushSample(1, channelRight[i] + currDelayLeft * feedback);
        
        if (currPoint == 0)
        {
            channelLeft[i] = (left + outputLeft * width) * gain;
            channelRight[i] = right * gain;
        }
        else
        {
            channelLeft[i] = left * gain;
            channelRight[i] = (right + outputRight * width) * gain;
        }

        /*float outputLeft = channelLeft[i] + wetDry * delayRight[currRead];
        float outputRight = channelRight[i] + wetDry * delayLeft[currRead];

        delayLeft[currWrite] = channelLeft[i] + (delayRight[currRead] * feedback);
        delayRight[currWrite] = channelRight[i] + (delayLeft[currRead] * feedback);

        auto max_size = delayTime / 1000 * getSampleRate() * getBlockSize();

        if (++currRead >= delayBufferSize) { currRead = 0; }
        if (++currWrite >= delayBufferSize) { currWrite = 0; }

        if (currPoint == 0)
        {
            channelLeft[i] = (left + outputLeft * width) * gain;
            channelRight[i] = right * gain;
        }
        else
        {
            channelLeft[i] = left * gain;
            channelRight[i] = (right + outputRight * width) * gain;
        }*/
    } 
}

//==============================================================================
bool PingPongDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PingPongDelayAudioProcessor::createEditor()
{
    return new PingPongDelayAudioProcessorEditor (*this);
}

//==============================================================================
void PingPongDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PingPongDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PingPongDelayAudioProcessor();
}
