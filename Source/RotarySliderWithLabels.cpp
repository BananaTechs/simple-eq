/*
  ==============================================================================

    RotarySliderWithLabels.cpp
    Created: 7 May 2024 3:29:30pm
    Author:  User

  ==============================================================================
*/

#include "RotarySliderWithLabels.h"

void LookAndFeel::drawRotarySlider(juce::Graphics& g,
                                   int x, int y, int width, int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);

    g.setColour(Colours::azure);
    g.fillEllipse(bounds);

    g.setColour(Colours::black);
    g.drawEllipse(bounds, 1.f);

    Path p;

    Rectangle<float> indicator;
    indicator.setLeft(bounds.getCentreX() - 2);
    indicator.setRight(bounds.getCentreX() + 2);
    indicator.setBottom(bounds.getCentreY());
    indicator.setTop(bounds.getY());

    p.addRectangle(indicator);

    jassert(rotaryStartAngle < rotaryEndAngle);

    auto sliderAngleRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

    p.applyTransform(AffineTransform().rotated(sliderAngleRad, bounds.getCentreX(), bounds.getCentreY()));

    g.fillPath(p);
}

//==============================================================================

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
    using namespace juce;

    auto startAngle = degreesToRadians(180.f + 30.f);
    auto endAngle = degreesToRadians(180.f - 30.f) + MathConstants<float>::twoPi;

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAngle,
                                      endAngle,
                                      *this);

    g.setColour(Colours::red);
    g.drawRect(getLocalBounds());
    g.setColour(Colours::yellow);
    g.drawRect(sliderBounds);
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    size -= getTextHeight() * 2;

    juce::Rectangle<int> sliderBounds;
    sliderBounds.setSize(size, size);
    sliderBounds.setCentre(bounds.getCentreX(), 0);
    sliderBounds.setY(2);

    return sliderBounds;
}

int RotarySliderWithLabels::getTextHeight() const
{
    return 14;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    return juce::String();
}
