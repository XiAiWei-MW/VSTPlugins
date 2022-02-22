/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PingPongDelayAudioProcessorEditor::PingPongDelayAudioProcessorEditor (PingPongDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 450);

    // Gain slider
    gainSlider.setSliderStyle(juce::Slider::LinearBar);
    gainSlider.setRange(0.0f, 1.0f, 0.1f);
    gainSlider.setPopupDisplayEnabled(true, false, this);
    gainSlider.setValue(1.0f);
    gainSlider.addListener(this);

    // Delay Time slider
    delayTimeSlider.setSliderStyle(juce::Slider::LinearBar);
    delayTimeSlider.setRange(0.0f, 2000.0f, 100.0f);
    delayTimeSlider.setPopupDisplayEnabled(true, false, this);
    delayTimeSlider.setValue(1000.0f);
    delayTimeSlider.addListener(this);

    // Feedback Slider
    feedbackSlider.setSliderStyle(juce::Slider::LinearBar);
    feedbackSlider.setRange(0.0f, 1.0f, 0.1f);
    feedbackSlider.setPopupDisplayEnabled(true, false, this);
    feedbackSlider.setValue(0.6f);
    feedbackSlider.addListener(this);

    // Visible
    addAndMakeVisible(&gainSlider);
    addAndMakeVisible(&delayTimeSlider);
    addAndMakeVisible(&feedbackSlider);
}

PingPongDelayAudioProcessorEditor::~PingPongDelayAudioProcessorEditor()
{
}

//==============================================================================
void PingPongDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void PingPongDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    gainSlider.setBounds(40, 50, getWidth() - 80, 40);
    delayTimeSlider.setBounds(40, 140, getWidth() - 80, 40);
    feedbackSlider.setBounds(40, 230, getWidth() - 80, 40);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    delayTimeLabel.setText("Delay Time (ms)", juce::dontSendNotification);
    delayTimeLabel.attachToComponent(&delayTimeSlider, false);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.attachToComponent(&feedbackSlider, false);

}

void PingPongDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &gainSlider)
    {
        //DBG("gain value changed");
        audioProcessor.gain = gainSlider.getValue();
    }
    else if (slider == &delayTimeSlider)
    {
        //DBG("delay time changed");
        auto newDelayTime = delayTimeSlider.getValue() / 1000.0;
        int newDelaySample = newDelayTime * 44100;

        audioProcessor.delayTime = newDelayTime;

        //int totalSample = newDelayTime * audioProcessor.getSampleRate() * audioProcessor.getBlockSize();
        //int newDelaySample = std::min(totalSample, audioProcessor.maxSample);
        DBG(newDelayTime << " " << newDelaySample << " " << audioProcessor.maxSample);
        //DBG(audioProcessor.getSampleRate() << " " << audioProcessor.getBlockSize());
        //DBG(audioProcessor.delayLine.getMaximumDelayInSamples());

        audioProcessor.delaySample = newDelaySample;
        audioProcessor.delayLine.setDelay(newDelaySample);
    }
    else if (slider == &feedbackSlider)
    {
        audioProcessor.feedback = feedbackSlider.getValue();
    }
}

void PingPongDelayAudioProcessorEditor::sliderDragEnded(juce::Slider* slider)
{
    
}
