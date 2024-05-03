/*
  ==============================================================================

    ResponseCurveComponent.h
    Created: 3 May 2024 4:25:50pm
    Author:  User

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class ResponseCurveComponent : public juce::Component,
    juce::AudioProcessorParameter::Listener,
    juce::Timer
{
public:
    SimpleEQAudioProcessor& audioProcessor;

    ResponseCurveComponent(SimpleEQAudioProcessor&);
    ~ResponseCurveComponent() override;

    void paint(juce::Graphics& g) override;

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override { }
    void timerCallback() override;

private:
    juce::Atomic<bool> parametersChanged{ false };

    MonoChain monoChain;
};