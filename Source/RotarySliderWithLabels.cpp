/*
  ==============================================================================

    RotarySliderWithLabels.cpp
    Created: 7 May 2024 3:29:30pm
    Author:  User

  ==============================================================================
*/

#include "RotarySliderWithLabels.h"

RotarySliderWithLabels::RotarySliderWithLabels(juce::RangedAudioParameter& param, const juce::String& unitSuffix) 
    : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                   juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(&param),
    suffix(unitSuffix)
{
    setLookAndFeel(&lnf);
}

RotarySliderWithLabels::~RotarySliderWithLabels()
{
    setLookAndFeel(nullptr);
}

void RotarySliderWithLabels::paint(juce::Graphics& g)
{

}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    return juce::Rectangle<int>();
}

int RotarySliderWithLabels::getTextHeight() const
{
    return 14;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    return juce::String();
}
