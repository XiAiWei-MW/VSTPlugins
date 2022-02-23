/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDelayAudioProcessorEditor::SimpleDelayAudioProcessorEditor (SimpleDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 450);

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

    // Width Slider
    wetSlider.setSliderStyle(juce::Slider::LinearBar);
    wetSlider.setRange(0.0f, 1.0f, 0.1f);
    wetSlider.setPopupDisplayEnabled(true, false, this);
    wetSlider.setValue(0.9f);
    wetSlider.addListener(this);

    // Visible
    addAndMakeVisible(&delayTimeSlider);
    addAndMakeVisible(&feedbackSlider);
    addAndMakeVisible(&wetSlider);
}

SimpleDelayAudioProcessorEditor::~SimpleDelayAudioProcessorEditor()
{
}

//==============================================================================
void SimpleDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void SimpleDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    delayTimeSlider.setBounds(40, 140, getWidth() - 80, 40);
    feedbackSlider.setBounds(40, 230, getWidth() - 80, 40);
    wetSlider.setBounds(40, 320, getWidth() - 80, 40);

    delayTimeLabel.setText("Delay Time (ms)", juce::dontSendNotification);
    delayTimeLabel.attachToComponent(&delayTimeSlider, false);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    wetLabel.setText("WetDry", juce::dontSendNotification);
    wetLabel.attachToComponent(&wetSlider, false);
}

void SimpleDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &delayTimeSlider)
    {
        auto newDelayTime = delayTimeSlider.getValue() / 1000.0;
        int newDelaySample = newDelayTime * 44100;

        audioProcessor.delayTime = newDelayTime;
        //DBG(newDelayTime << " " << newDelaySample << " " << audioProcessor.maxSample);

        audioProcessor.delaySample = newDelaySample;
        audioProcessor.delayLine.setDelay(newDelaySample);
    }
    else if (slider == &feedbackSlider)
    {
        audioProcessor.feedback = feedbackSlider.getValue();
    }
    else if (slider == &wetSlider)
    {
        audioProcessor.wetDry = wetSlider.getValue();
    }
}
