#pragma once

#include <JuceHeader.h>

namespace Parameters
{

//==============================================================================
// Parameter IDs
//==============================================================================

namespace ID
{
    static const juce::String linked        = "linked";
    static const juce::String bypass        = "bypass";
    static const juce::String inputGain     = "inputGain";
    static const juce::String threshold     = "threshold";
    static const juce::String ratio         = "ratio";
    static const juce::String kneeWidth     = "kneeWidth";
    static const juce::String outputGain    = "outputGain";
    static const juce::String attackTime    = "attackTime";
    static const juce::String releaseTime   = "releaseTime";
    static const juce::String holdTime      = "holdTime";
    static const juce::String lookaheadTime = "lookaheadTime";
}

//==============================================================================
// Parameter Labels
//==============================================================================

namespace Label
{
    static const juce::String linked        = "Linked";
    static const juce::String bypass        = "Bypass";
    static const juce::String inputGain     = "Input Gain";
    static const juce::String threshold     = "Threshold";
    static const juce::String ratio         = "Ratio";
    static const juce::String kneeWidth     = "Knee Width";
    static const juce::String outputGain    = "Output Gain";
    static const juce::String attackTime    = "Attack";
    static const juce::String releaseTime   = "Release";
    static const juce::String holdTime      = "Hold";
    static const juce::String lookaheadTime = "Lookahead";
}

//==============================================================================
// Parameter Ranges (matching FBAudio specs)
//==============================================================================

namespace Range
{
    // Input/Output Gain: -30 to +30 dB
    static const juce::NormalisableRange<float> gain(-30.0f, 30.0f, 0.1f);

    // Threshold: -96 to 0 dB
    static const juce::NormalisableRange<float> threshold(-96.0f, 0.0f, 0.1f);

    // Ratio: 1:1 to 100:1 (with skew for better control at low ratios)
    static const juce::NormalisableRange<float> ratio(1.0f, 100.0f, 0.1f, 0.3f);

    // Knee Width: 0 to 48 dB
    static const juce::NormalisableRange<float> kneeWidth(0.0f, 48.0f, 0.1f);

    // Attack Time: 0 to 1000 ms
    static const juce::NormalisableRange<float> attackTime(0.0f, 1000.0f, 0.1f, 0.4f);

    // Release Time: 0 to 1000 ms
    static const juce::NormalisableRange<float> releaseTime(0.0f, 1000.0f, 0.1f, 0.4f);

    // Hold Time: 0 to 1000 ms
    static const juce::NormalisableRange<float> holdTime(0.0f, 1000.0f, 0.1f, 0.4f);

    // Lookahead Time: 0 to 30 ms
    static const juce::NormalisableRange<float> lookaheadTime(0.0f, 30.0f, 0.1f);
}

//==============================================================================
// Default Values (matching FBAudio specs)
//==============================================================================

namespace Default
{
    static const bool  linked        = true;
    static const bool  bypass        = false;
    static const float inputGain     = 0.0f;
    static const float threshold     = 0.0f;
    static const float ratio         = 1.0f;
    static const float kneeWidth     = 0.0f;
    static const float outputGain    = 0.0f;
    static const float attackTime    = 0.0f;
    static const float releaseTime   = 100.0f;
    static const float holdTime      = 0.0f;
    static const float lookaheadTime = 0.0f;
}

//==============================================================================
// Helper function to create the parameter layout
//==============================================================================

inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Boolean parameters
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ID::linked, 1),
        Label::linked,
        Default::linked));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ID::bypass, 1),
        Label::bypass,
        Default::bypass));

    // Gain parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID::inputGain, 1),
        Label::inputGain,
        Range::gain,
        Default::inputGain,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID::outputGain, 1),
        Label::outputGain,
        Range::gain,
        Default::outputGain,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Compression parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID::threshold, 1),
        Label::threshold,
        Range::threshold,
        Default::threshold,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID::ratio, 1),
        Label::ratio,
        Range::ratio,
        Default::ratio,
        juce::AudioParameterFloatAttributes().withLabel(":1")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID::kneeWidth, 1),
        Label::kneeWidth,
        Range::kneeWidth,
        Default::kneeWidth,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Time parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID::attackTime, 1),
        Label::attackTime,
        Range::attackTime,
        Default::attackTime,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID::releaseTime, 1),
        Label::releaseTime,
        Range::releaseTime,
        Default::releaseTime,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID::holdTime, 1),
        Label::holdTime,
        Range::holdTime,
        Default::holdTime,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID::lookaheadTime, 1),
        Label::lookaheadTime,
        Range::lookaheadTime,
        Default::lookaheadTime,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    return { params.begin(), params.end() };
}

} // namespace Parameters
