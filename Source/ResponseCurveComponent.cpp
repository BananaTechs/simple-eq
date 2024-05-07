/*
  ==============================================================================

    ResponseCurveComponent.cpp
    Created: 3 May 2024 4:25:50pm
    Author:  User

  ==============================================================================
*/

#include "ResponseCurveComponent.h"

ResponseCurveComponent::ResponseCurveComponent(SimpleEQAudioProcessor& p)
    : audioProcessor(p)
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }

    startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }
}

void ResponseCurveComponent::paint(juce::Graphics& g)
{
    using namespace juce;

    auto width = getWidth();

    auto& lowCut = monoChain.get<ChainPositions::LOW_CUT_FILTER>();
    auto& peakFilter = monoChain.get<ChainPositions::PEAK_FILTER>();
    auto& highCut = monoChain.get<ChainPositions::HIGH_CUT_FILTER>();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> magnitudes;
    magnitudes.resize(width);

    for (int i = 0; i < width; i++)
    {
        double mag = 1.0;
        auto freq = mapToLog10<double>(double(i) / double(width), 20.0, 20000.0);

        if (!monoChain.isBypassed<ChainPositions::PEAK_FILTER>())
            mag *= peakFilter.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<0>())
            mag *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<1>())
            mag *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<2>())
            mag *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<3>())
            mag *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highCut.isBypassed<0>())
            mag *= highCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highCut.isBypassed<1>())
            mag *= highCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highCut.isBypassed<2>())
            mag *= highCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highCut.isBypassed<3>())
            mag *= highCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        magnitudes[i] = Decibels::gainToDecibels(mag);
    }

    Path responseCurve;

    const double outputMin = getBottom();
    const double outputMax = getY();
    auto map = [outputMin, outputMax](double input)
        {
            return jmap(input, -27.0, 27.0, outputMin, outputMax);
        };

    responseCurve.startNewSubPath(getX(), map(magnitudes.front()));

    for (int i = 1; i < magnitudes.size(); i += 1)
    {
        responseCurve.lineTo(getX() + i, map(magnitudes[i]));
    }

    g.setColour(Colours::aquamarine);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 4.0, 2.0);

    g.setColour(Colours::azure);
    g.strokePath(responseCurve, PathStrokeType(2.0));
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

void ResponseCurveComponent::timerCallback()
{
    if (parametersChanged.compareAndSetBool(false, true))
    {
        audioProcessor.updateMonoChain(monoChain);
        repaint();
    }
}
