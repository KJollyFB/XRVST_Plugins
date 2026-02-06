#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/Utilities.h"

//==============================================================================
BroadbandCompressorAudioProcessor::BroadbandCompressorAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters_(*this, nullptr, juce::Identifier("XRVST_Plugins"),
                  Parameters::createParameterLayout())
{
    // Cache parameter pointers for efficient access
    linkedParam_ = parameters_.getRawParameterValue(Parameters::ID::linked);
    bypassParam_ = parameters_.getRawParameterValue(Parameters::ID::bypass);
    inputGainParam_ = parameters_.getRawParameterValue(Parameters::ID::inputGain);
    thresholdParam_ = parameters_.getRawParameterValue(Parameters::ID::threshold);
    ratioParam_ = parameters_.getRawParameterValue(Parameters::ID::ratio);
    kneeWidthParam_ = parameters_.getRawParameterValue(Parameters::ID::kneeWidth);
    outputGainParam_ = parameters_.getRawParameterValue(Parameters::ID::outputGain);
    attackTimeParam_ = parameters_.getRawParameterValue(Parameters::ID::attackTime);
    releaseTimeParam_ = parameters_.getRawParameterValue(Parameters::ID::releaseTime);
    holdTimeParam_ = parameters_.getRawParameterValue(Parameters::ID::holdTime);
    lookaheadTimeParam_ = parameters_.getRawParameterValue(Parameters::ID::lookaheadTime);
}

BroadbandCompressorAudioProcessor::~BroadbandCompressorAudioProcessor()
{
}

//==============================================================================
const juce::String BroadbandCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BroadbandCompressorAudioProcessor::acceptsMidi() const
{
    return false;
}

bool BroadbandCompressorAudioProcessor::producesMidi() const
{
    return false;
}

bool BroadbandCompressorAudioProcessor::isMidiEffect() const
{
    return false;
}

double BroadbandCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BroadbandCompressorAudioProcessor::getNumPrograms()
{
    return 1;
}

int BroadbandCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BroadbandCompressorAudioProcessor::setCurrentProgram(int /*index*/)
{
}

const juce::String BroadbandCompressorAudioProcessor::getProgramName(int /*index*/)
{
    return {};
}

void BroadbandCompressorAudioProcessor::changeProgramName(int /*index*/, const juce::String& /*newName*/)
{
}

//==============================================================================
void BroadbandCompressorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate_ = sampleRate;

    // Initialize level estimator
    levelEstimator_.setSampleRate(static_cast<float>(sampleRate));
    levelEstimator_.setAttackTimeMs(*attackTimeParam_);
    levelEstimator_.setReleaseTimeMs(*releaseTimeParam_);
    levelEstimator_.setHoldTimeMs(*holdTimeParam_);

    // Initialize delay lines for each channel
    int numChannels = getTotalNumInputChannels();
    delayLines_.resize(numChannels);
    levelStates_.resize(numChannels);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        delayLines_[ch].prepare(kMaxLookaheadSamples);
        delayLines_[ch].setDelayMs(*lookaheadTimeParam_, static_cast<float>(sampleRate));
        levelStates_[ch].reset();
    }

    // Allocate level buffer
    levelBuffer_.setSize(1, samplesPerBlock);

    // Initialize gain smoothing
    inputGainSmoothed_.reset(sampleRate, 0.02);  // 20ms smoothing
    outputGainSmoothed_.reset(sampleRate, 0.02);

    // Update compression curve parameters
    updateParameters();
}

void BroadbandCompressorAudioProcessor::releaseResources()
{
    // Clear delay lines
    for (auto& delay : delayLines_)
        delay.reset();

    // Clear level states
    for (auto& state : levelStates_)
        state.reset();
}

bool BroadbandCompressorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Support mono and stereo configurations
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Input and output must match
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void BroadbandCompressorAudioProcessor::updateParameters()
{
    // Update level estimator
    levelEstimator_.setAttackTimeMs(*attackTimeParam_);
    levelEstimator_.setReleaseTimeMs(*releaseTimeParam_);
    levelEstimator_.setHoldTimeMs(*holdTimeParam_);

    // Update compression curve
    compressionCurve_.setThresholdDb(*thresholdParam_);
    compressionCurve_.setCompressionRatio(*ratioParam_);
    compressionCurve_.setKneeWidthDb(*kneeWidthParam_);
    compressionCurve_.setMakeupGainDb(*outputGainParam_);

    // Update delay lines
    for (auto& delay : delayLines_)
        delay.setDelayMs(*lookaheadTimeParam_, static_cast<float>(currentSampleRate_));

    // Update gain smoothing targets
    inputGainSmoothed_.setTargetValue(DSP::db2Mag(*inputGainParam_));
    outputGainSmoothed_.setTargetValue(DSP::db2Mag(*outputGainParam_));
}

void BroadbandCompressorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                        juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();

    // Clear any extra output channels
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    // Check bypass
    if (*bypassParam_ > 0.5f)
    {
        currentGainReductionDb_.store(0.0f);
        return;
    }

    // Update parameters (could be automated)
    updateParameters();

    // Report latency for lookahead
    int lookaheadSamples = static_cast<int>(*lookaheadTimeParam_ * currentSampleRate_ / 1000.0f);
    setLatencySamples(lookaheadSamples);

    // Process compression
    processCompression(buffer);
}

void BroadbandCompressorAudioProcessor::processCompression(juce::AudioBuffer<float>& buffer)
{
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    bool linked = *linkedParam_ > 0.5f;

    // Ensure we have enough channels
    if (numChannels > static_cast<int>(delayLines_.size()))
        return;

    // Resize level buffer if needed
    if (levelBuffer_.getNumSamples() < numSamples)
        levelBuffer_.setSize(1, numSamples, false, false, true);

    // Clear level buffer for linked mode
    if (linked)
        levelBuffer_.clear();

    // Calculate input level for metering
    float maxInputLevel = 0.0f;

    // Phase 1: Apply input gain and estimate levels
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);

        // Apply input gain
        for (int i = 0; i < numSamples; ++i)
        {
            float gain = inputGainSmoothed_.getNextValue();
            channelData[i] *= gain;

            // Track input level
            float absLevel = std::fabs(channelData[i]);
            if (absLevel > maxInputLevel)
                maxInputLevel = absLevel;
        }

        // Estimate level for this channel
        float* levelData = levelBuffer_.getWritePointer(0);

        if (linked)
        {
            // Linked mode: accumulate max level across channels
            levelEstimator_.estimateLevelLinked(channelData, levelData, numSamples,
                                                 levelStates_[ch], ch > 0);
        }
        else
        {
            // Independent mode: estimate for this channel only
            levelEstimator_.estimateLevel(channelData, levelData, numSamples, levelStates_[ch]);
        }
    }

    // Update input level meter
    currentInputLevelDb_.store(DSP::mag2Db(maxInputLevel));

    // Phase 2: Apply compression
    float maxGainReduction = 0.0f;
    float maxOutputLevel = 0.0f;
    float* levelData = levelBuffer_.getWritePointer(0);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);

        // If not linked, we need to recalculate level for this channel
        if (!linked && ch > 0)
        {
            levelEstimator_.estimateLevel(channelData, levelData, numSamples, levelStates_[ch]);
        }

        for (int i = 0; i < numSamples; ++i)
        {
            // Get level estimate
            float level = levelData[i];

            // Calculate compression gain
            float gainDb = compressionCurve_.calculateCompressionGainDb(DSP::mag2Db(level));
            float gain = DSP::db2Mag(gainDb);

            // Track max gain reduction
            if (gainDb < maxGainReduction)
                maxGainReduction = gainDb;

            // Apply lookahead delay
            float delayedSample = delayLines_[ch].processSample(channelData[i]);

            // Apply compression gain and output gain
            float outputGain = outputGainSmoothed_.getNextValue();
            float outputSample = delayedSample * gain * outputGain;

            channelData[i] = outputSample;

            // Track output level
            float absOutput = std::fabs(outputSample);
            if (absOutput > maxOutputLevel)
                maxOutputLevel = absOutput;
        }
    }

    // Update meters
    currentGainReductionDb_.store(maxGainReduction);
    currentOutputLevelDb_.store(DSP::mag2Db(maxOutputLevel));
}

//==============================================================================
bool BroadbandCompressorAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* BroadbandCompressorAudioProcessor::createEditor()
{
    return new BroadbandCompressorAudioProcessorEditor(*this);
}

//==============================================================================
void BroadbandCompressorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BroadbandCompressorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters_.state.getType()))
        {
            parameters_.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BroadbandCompressorAudioProcessor();
}
