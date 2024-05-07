/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), responseCurve(p),
    peakFreqSlider(*audioProcessor.apvts.getParameter(PEAK_FREQ), "Hz"),
    peakGainSlider(*audioProcessor.apvts.getParameter(PEAK_GAIN), "dB"),
    peakQualitySlider(*audioProcessor.apvts.getParameter(PEAK_QUALITY), ""),
    lowCutFreqSlider(*audioProcessor.apvts.getParameter(LOW_CUT), "Hz"),
    highCutFreqSlider(*audioProcessor.apvts.getParameter(HIGH_CUT), "Hz"),
    lowCutSlopeSlider(*audioProcessor.apvts.getParameter(LOW_GAIN), "dB/Oct"),
    highCutSlopeSlider(*audioProcessor.apvts.getParameter(HIGH_GAIN), "dB/Oct"),
    peakFreqSliderAttachment(audioProcessor.apvts, PEAK_FREQ, peakFreqSlider),
    peakGainSliderAttachment(audioProcessor.apvts, PEAK_GAIN, peakGainSlider),
    peakQualitySliderAttachment(audioProcessor.apvts, PEAK_QUALITY, peakQualitySlider),
    lowCutFreqSliderAttachment(audioProcessor.apvts, LOW_CUT, lowCutFreqSlider),
    highCutFreqSliderAttachment(audioProcessor.apvts, HIGH_CUT, highCutFreqSlider),
    lowCutSlopeSliderAttachment(audioProcessor.apvts, LOW_GAIN, lowCutSlopeSlider),
    highCutSlopeSliderAttachment(audioProcessor.apvts, HIGH_GAIN, highCutSlopeSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto* component : components)
    {
        addAndMakeVisible(component);
    }

    createLabels();

    setSize(400, 500);
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
}

void SimpleEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    responseCurve.setBounds(responseArea);

    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);

    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds(bounds);
}

void SimpleEQAudioProcessorEditor::createLabels()
{
    peakFreqSlider.labels.add(RotarySliderWithLabels::LabelPos{0.f, "20 Hz"});
    peakFreqSlider.labels.add(RotarySliderWithLabels::LabelPos{ 1.f, "20 kHz" });

    peakGainSlider.labels.add(RotarySliderWithLabels::LabelPos{ 0.f, "-24 dB" });
    peakGainSlider.labels.add(RotarySliderWithLabels::LabelPos{ 1.f, "24 dB" });

    peakQualitySlider.labels.add(RotarySliderWithLabels::LabelPos{ 0.f, "0.1" });
    peakQualitySlider.labels.add(RotarySliderWithLabels::LabelPos{ 1.f, "10" });

    lowCutFreqSlider.labels.add(RotarySliderWithLabels::LabelPos{ 0.f, "20 Hz" });
    lowCutFreqSlider.labels.add(RotarySliderWithLabels::LabelPos{ 1.f, "20 kHz" });

    highCutFreqSlider.labels.add(RotarySliderWithLabels::LabelPos{ 0.f, "20 Hz" });
    highCutFreqSlider.labels.add(RotarySliderWithLabels::LabelPos{ 1.f, "20 kHz" });

    lowCutSlopeSlider.labels.add(RotarySliderWithLabels::LabelPos{ 0.f, "12 dB" });
    lowCutSlopeSlider.labels.add(RotarySliderWithLabels::LabelPos{ 1.f, "48 dB" });

    highCutSlopeSlider.labels.add(RotarySliderWithLabels::LabelPos{ 0.f, "12 dB" });
    highCutSlopeSlider.labels.add(RotarySliderWithLabels::LabelPos{ 1.f, "48 dB" });
}
