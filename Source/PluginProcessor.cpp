/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessor::SimpleEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    lowCutFreq = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(LOW_CUT));
    highCutFreq = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(HIGH_CUT));
    lowCutGain = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(LOW_GAIN));
    highCutGain = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(HIGH_GAIN));
    peakFreq = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(PEAK_FREQ));
    peakGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(PEAK_GAIN));
    peakQuality = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(PEAK_QUALITY));

    jassert(lowCutFreq != nullptr);
    jassert(highCutFreq != nullptr);
    jassert(lowCutGain != nullptr);
    jassert(highCutGain != nullptr);
    jassert(peakFreq != nullptr);
    jassert(peakGain != nullptr);
    jassert(peakQuality != nullptr);
}

SimpleEQAudioProcessor::~SimpleEQAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec{ sampleRate, samplesPerBlock, 1 };
    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

void SimpleEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateMonoChains(std::vector<MonoChain*>{&leftChain, &rightChain});

    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

void SimpleEQAudioProcessor::updateMonoChain(MonoChain& monoChain)
{
    updateMonoChains(std::vector<MonoChain*>{ &monoChain });
}

void SimpleEQAudioProcessor::updateMonoChains(std::vector<MonoChain*>& monoChains)
{
    updatePeakFilter(monoChains);
    updateCutFilters(monoChains);
}

void SimpleEQAudioProcessor::updatePeakFilter(std::vector<MonoChain*>& monoChains)
{
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        getSampleRate(),
        peakFreq->get(),
        peakQuality->get(),
        juce::Decibels::decibelsToGain(peakGain->get()));

    for (MonoChain* monoChain : monoChains)
    {
        *monoChain->get<ChainPositions::PEAK_FILTER>().coefficients = *peakCoefficients;
    }
}

void SimpleEQAudioProcessor::updateCutFilters(std::vector<MonoChain*>& monoChains)
{
    auto lowCutGainIndex = lowCutGain->getIndex();
    auto highCutGainIndex = highCutGain->getIndex();

    auto lowPassOrder = (lowCutGainIndex + 1) * 2;
    auto highPassOrder = (highCutGainIndex + 1) * 2;

    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(lowCutFreq->get(),
        getSampleRate(),
        lowPassOrder);
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(highCutFreq->get(),
        getSampleRate(),
        highPassOrder);

    for (MonoChain* monoChain : monoChains)
    {
        auto& lowCut = monoChain->get<ChainPositions::LOW_CUT_FILTER>();
        auto& highCut = monoChain->get<ChainPositions::HIGH_CUT_FILTER>();

        setCutFilterParams(lowCut, lowCutCoefficients, lowCutGainIndex);
        setCutFilterParams(highCut, highCutCoefficients, highCutGainIndex);
    }
}

void SimpleEQAudioProcessor::setCutFilterParams(CutFilter& filterChain, 
                                             juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>>& cutCoefficients,
                                             int numFilters)
{
    jassert(numFilters <= cutCoefficients.size());

    filterChain.setBypassed<0>(true);
    filterChain.setBypassed<1>(true);
    filterChain.setBypassed<2>(true);
    filterChain.setBypassed<3>(true);

    switch (numFilters)
    {
    case 3:
        updateAndEnableFilter<3>(filterChain, cutCoefficients);
    case 2:
        updateAndEnableFilter<2>(filterChain, cutCoefficients);
    case 1:
        updateAndEnableFilter<1>(filterChain, cutCoefficients);
    case 0:
        updateAndEnableFilter<0>(filterChain, cutCoefficients);
        break;
    }
}

//==============================================================================
bool SimpleEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleEQAudioProcessor::createEditor()
{
    //return new juce::GenericAudioProcessorEditor (*this);
    return new SimpleEQAudioProcessorEditor(*this);
}

//==============================================================================
void SimpleEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void SimpleEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleEQAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;

    auto gainChoices = std::vector<double>{ 12, 24, 36, 48 };
    juce::StringArray sa;
    for (auto choice : gainChoices)
    {
        sa.add(juce::String(choice, 1));
    }

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        LOW_CUT, 
        LOW_CUT, 
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 
        20));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        LOW_GAIN,
        LOW_GAIN,
        sa,
        1));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        HIGH_CUT,
        HIGH_CUT,
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
        20000));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        HIGH_GAIN,
        HIGH_GAIN,
        sa,
        1));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        PEAK_FREQ,
        PEAK_FREQ,
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
        3000));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        PEAK_GAIN,
        PEAK_GAIN,
        juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
        0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        PEAK_QUALITY,
        PEAK_QUALITY,
        juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 0.33f),
        1));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQAudioProcessor();
}
