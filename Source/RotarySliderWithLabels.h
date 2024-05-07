/*
  ==============================================================================

    RotarySliderWithLabels.h
    Created: 7 May 2024 3:29:30pm
    Author:  User

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics&,
                          int x, int y, int width, int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider&) override;
};

class RotarySliderWithLabels : public juce::Slider
{
public:
    RotarySliderWithLabels(juce::RangedAudioParameter& param, const juce::String& unitSuffix);
    ~RotarySliderWithLabels();

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const;
    juce::String getDisplayString() const;

private:
    LookAndFeel lnf;

    juce::RangedAudioParameter* param;
    juce::String suffix;
};