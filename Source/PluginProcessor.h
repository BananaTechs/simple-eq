/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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
    inline static const juce::String LOW_CUT = "Low Cut";
    inline static const juce::String HIGH_CUT = "High Cut";
    inline static const juce::String LOW_GAIN = "Low Gain";
    inline static const juce::String HIGH_GAIN = "High Gain";
    inline static const juce::String PEAK_FREQ = "Peak Freq";
    inline static const juce::String PEAK_GAIN = "Peak Gain";
    inline static const juce::String PEAK_QUALITY = "Peak Quality";
    
    juce::AudioParameterFloat* lowCutFreq{ nullptr };
    juce::AudioParameterFloat* highCutFreq{ nullptr };
    juce::AudioParameterChoice* lowCutGain{ nullptr };
    juce::AudioParameterChoice* highCutGain{ nullptr };
    juce::AudioParameterFloat* peakFreq{ nullptr };
    juce::AudioParameterFloat* peakGain{ nullptr };
    juce::AudioParameterFloat* peakQuality{ nullptr };

    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    MonoChain leftChain, rightChain;

    enum ChainPositions
    {
        LOW_CUT_FILTER,
        PEAK_FILTER,
        HIGH_CUT_FILTER
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};