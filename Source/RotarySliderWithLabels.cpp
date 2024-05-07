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

    RotarySliderWithLabels* rswl;

    if (!(rswl = dynamic_cast<RotarySliderWithLabels*>(&slider)))
    {
        return;
    }

    auto bounds = Rectangle<float>(x, y, width, height);

    g.setColour(Colours::azure);
    g.fillEllipse(bounds);

    g.setColour(findColour(juce::ResizableWindow::backgroundColourId));
    g.drawEllipse(bounds, 1.f);

    Path p;

    Rectangle<float> indicator;
    indicator.setLeft(bounds.getCentreX() - 2);
    indicator.setRight(bounds.getCentreX() + 2);
    indicator.setBottom(bounds.getCentreY());
    indicator.setTop(bounds.getY());

    p.addRoundedRectangle(indicator, 2.f);

    jassert(rotaryStartAngle < rotaryEndAngle);

    auto sliderAngleRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

    p.applyTransform(AffineTransform().rotated(sliderAngleRad, bounds.getCentreX(), bounds.getCentreY()));

    g.fillPath(p);

    g.setFont(rswl->getTextHeight());
    auto text = rswl->getDisplayString();
    auto strWidth = g.getCurrentFont().getStringWidth(text);
    
    Rectangle<float> textBounds;
    textBounds.setSize(strWidth + 8, rswl->getTextHeight() + 4);
    textBounds.setCentre(bounds.getCentre());

    g.fillRoundedRectangle(textBounds, 4.f);

    g.setColour(Colours::azure);
    g.drawFittedText(text, textBounds.toNearestInt(), Justification::centred, 1);
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

    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;

    g.setColour(Colours::aquamarine);
    g.setFont(getTextHeight());

    for (int i = 0; i < labels.size(); i++)
    {
        auto pos = labels[i].pos;
        jassert(pos >= 0.f);
        jassert(pos <= 1.f);

        auto angle = jmap(pos, 0.f, 1.f, startAngle, endAngle);
        auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, angle);

        Rectangle<float> textBounds;
        auto str = labels[i].label;
        auto strWidth = g.getCurrentFont().getStringWidth(str);
        textBounds.setSize(strWidth, getTextHeight());
        textBounds.setCentre(c);
        textBounds.setY(textBounds.getY() + getTextHeight());

        g.drawFittedText(str, textBounds.toNearestInt(), Justification::centred, 1);
    }

    // Draws bounding boxes for reference
    /*g.setColour(Colours::red);
    g.drawRect(getLocalBounds());
    g.setColour(Colours::yellow);
    g.drawRect(sliderBounds);*/
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    size -= getTextHeight() * 2;

    juce::Rectangle<int> sliderBounds;
    sliderBounds.setSize(size, size);
    sliderBounds.setCentre(bounds.getCentreX(), 0);
    sliderBounds.setY(8);

    return sliderBounds;
}

int RotarySliderWithLabels::getTextHeight() const
{
    return 14;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
    {
        auto str = choiceParam->getCurrentChoiceName();
        str << " ";
        str << suffix;
        return str;
    }

    juce::String str;
    bool addK = false;

    juce::AudioParameterFloat* floatParam;
    if (!(floatParam = dynamic_cast<juce::AudioParameterFloat*>(param)))
    {
        jassertfalse;
    }

    float val = getValue();
    if (val >= 1000.f)
    {
        val /= 1000.f;
        addK = true;
    }

    str = juce::String(val, addK ? 2 : 0);

    if (suffix.isEmpty())
    {
        return str;
    }

    str << " ";
    if (addK)
    {
        str << "k";
    }
    str << suffix;

    return str;
}
