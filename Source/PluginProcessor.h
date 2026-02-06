#pragma once

#include <JuceHeader.h>
#include "Parameters.h"
#include "DSP/LevelEstimator.h"
#include "DSP/CompressionCurve.h"
#include "DSP/DelayLine.h"

//==============================================================================
/**
    Broadband Compressor V2 Audio Processor

    Implements a standard dynamic range compressor with:
    - Soft-knee compression curve
    - Attack, release, and hold envelope detection
    - Lookahead for transient-aware compression
    - Linked stereo mode for image preservation
*/
class BroadbandCompressorAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    BroadbandCompressorAudioProcessor();
    ~BroadbandCompressorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access
    juce::AudioProcessorValueTreeState& getValueTreeState() { return parameters_; }

    // Metering (thread-safe access for GUI)
    float getGainReductionDb() const { return currentGainReductionDb_.load(); }
    float getInputLevelDb() const { return currentInputLevelDb_.load(); }
    float getOutputLevelDb() const { return currentOutputLevelDb_.load(); }

    // For transfer curve visualization
    const DSP::CompressionCurve& getCompressionCurve() const { return compressionCurve_; }

private:
    //==============================================================================
    void updateParameters();
    void processCompression(juce::AudioBuffer<float>& buffer);

    //==============================================================================
    // Parameters
    juce::AudioProcessorValueTreeState parameters_;

    // Cached parameter values
    std::atomic<float>* linkedParam_ = nullptr;
    std::atomic<float>* bypassParam_ = nullptr;
    std::atomic<float>* inputGainParam_ = nullptr;
    std::atomic<float>* thresholdParam_ = nullptr;
    std::atomic<float>* ratioParam_ = nullptr;
    std::atomic<float>* kneeWidthParam_ = nullptr;
    std::atomic<float>* outputGainParam_ = nullptr;
    std::atomic<float>* attackTimeParam_ = nullptr;
    std::atomic<float>* releaseTimeParam_ = nullptr;
    std::atomic<float>* holdTimeParam_ = nullptr;
    std::atomic<float>* lookaheadTimeParam_ = nullptr;

    // DSP components
    DSP::LevelEstimator levelEstimator_;
    DSP::CompressionCurve compressionCurve_;
    std::vector<DSP::DelayLine> delayLines_;
    std::vector<DSP::LevelEstimator::State> levelStates_;

    // Processing buffers
    juce::AudioBuffer<float> levelBuffer_;

    // Gain smoothing
    juce::SmoothedValue<float> inputGainSmoothed_;
    juce::SmoothedValue<float> outputGainSmoothed_;

    // Metering
    std::atomic<float> currentGainReductionDb_{ 0.0f };
    std::atomic<float> currentInputLevelDb_{ -100.0f };
    std::atomic<float> currentOutputLevelDb_{ -100.0f };

    // Sample rate
    double currentSampleRate_ = 44100.0;

    // Max lookahead delay samples (30ms at 192kHz)
    static constexpr size_t kMaxLookaheadSamples = 5760;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BroadbandCompressorAudioProcessor)
};
