/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

const juce::String LOW_CUT = "Low Cut";
const juce::String HIGH_CUT = "High Cut";
const juce::String LOW_GAIN = "Low Gain";
const juce::String HIGH_GAIN = "High Gain";
const juce::String PEAK_FREQ = "Peak Freq";
const juce::String PEAK_GAIN = "Peak Gain";
const juce::String PEAK_QUALITY = "Peak Quality";

using Filter = juce::dsp::IIR::Filter<float>;
using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

enum ChainPositions
{
    LOW_CUT_FILTER,
    PEAK_FILTER,
    HIGH_CUT_FILTER
};

//==============================================================================
/**
*/
class SimpleEQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleEQAudioProcessor();
    ~SimpleEQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();
    APVTS apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    juce::AudioParameterFloat* lowCutFreq{ nullptr };
    juce::AudioParameterFloat* highCutFreq{ nullptr };
    juce::AudioParameterChoice* lowCutGain{ nullptr };
    juce::AudioParameterChoice* highCutGain{ nullptr };
    juce::AudioParameterFloat* peakFreq{ nullptr };
    juce::AudioParameterFloat* peakGain{ nullptr };
    juce::AudioParameterFloat* peakQuality{ nullptr };

    MonoChain leftChain, rightChain;

    void updatePeakFilter();
    void updateCutFilters();
    void setCutFilterParams(CutFilter& filterChain,
                         juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>>& cutCoefficients,
                         int numFilters);

    template<int Index, typename CoefficientType>
    void updateAndEnableFilter(CutFilter& filterChain, CoefficientType& coefficients)
    {
        *filterChain.get<Index>().coefficients = *coefficients[Index];
        filterChain.setBypassed<Index>(false);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};
