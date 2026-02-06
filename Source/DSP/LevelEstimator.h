#pragma once

#include "Utilities.h"
#include <cstddef>
#include <cmath>
#include <algorithm>

namespace DSP
{

/**
    Envelope follower with attack, release, and hold times.

    This class estimates the level of an audio signal using a peak detector
    with configurable attack, hold, and release characteristics.
*/
class LevelEstimator
{
public:
    /** State for each channel's level estimation. */
    struct State
    {
        void reset()
        {
            levelEstimate = 0.0f;
            holdCount = 0;
        }

        float levelEstimate = 0.0f;
        size_t holdCount = 0;
    };

    LevelEstimator() = default;

    explicit LevelEstimator(float sampleRateHz)
        : sampleRateHz_(sampleRateHz)
    {
    }

    //==============================================================================
    // Sample rate

    void setSampleRate(float sampleRateHz)
    {
        if (sampleRateHz <= 0.0f)
            return;

        if (sampleRateHz != sampleRateHz_)
        {
            sampleRateHz_ = sampleRateHz;
            updateCoefficients();
        }
    }

    float getSampleRate() const { return sampleRateHz_; }

    //==============================================================================
    // Attack time

    void setAttackTimeMs(float attackTimeMs)
    {
        if (attackTimeMs < 0.0f)
            return;

        attackTimeSec_ = attackTimeMs / 1000.0f;
        attackCoefficient_ = timeConstantToCoefficient(attackTimeSec_, sampleRateHz_);
    }

    float getAttackTimeMs() const { return attackTimeSec_ * 1000.0f; }
    float getAttackCoefficient() const { return attackCoefficient_; }

    //==============================================================================
    // Release time

    void setReleaseTimeMs(float releaseTimeMs)
    {
        if (releaseTimeMs < 0.0f)
            return;

        releaseTimeSec_ = releaseTimeMs / 1000.0f;
        releaseCoefficient_ = timeConstantToCoefficient(releaseTimeSec_, sampleRateHz_);
    }

    float getReleaseTimeMs() const { return releaseTimeSec_ * 1000.0f; }
    float getReleaseCoefficient() const { return releaseCoefficient_; }

    //==============================================================================
    // Hold time

    void setHoldTimeMs(float holdTimeMs)
    {
        if (holdTimeMs < 0.0f)
            return;

        holdTimeSec_ = holdTimeMs / 1000.0f;
        holdTimeSamples_ = static_cast<size_t>(holdTimeSec_ * sampleRateHz_);
    }

    float getHoldTimeMs() const { return holdTimeSec_ * 1000.0f; }
    size_t getHoldTimeSamples() const { return holdTimeSamples_; }

    //==============================================================================
    // Processing

    /** Process a buffer of audio samples and estimate the level.

        @param input        Pointer to input samples
        @param levelOutput  Pointer to output level estimates (same size as input)
        @param numSamples   Number of samples to process
        @param state        State for this channel (maintains envelope between calls)
    */
    void estimateLevel(const float* input,
                       float* levelOutput,
                       size_t numSamples,
                       State& state) const
    {
        for (size_t i = 0; i < numSamples; ++i)
        {
            float level = std::fabs(input[i]);

            if (level > state.levelEstimate)
            {
                // Attack phase: level is increasing
                state.levelEstimate += (level - state.levelEstimate) * attackCoefficient_;
                state.holdCount = 0;
            }
            else if (state.holdCount < holdTimeSamples_)
            {
                // Hold phase: maintain current level
                state.holdCount++;
            }
            else
            {
                // Release phase: level is decreasing
                state.levelEstimate += (level - state.levelEstimate) * releaseCoefficient_;
            }

            levelOutput[i] = state.levelEstimate;
        }
    }

    /** Process a buffer with linked mode (use max level across channels).

        @param input        Pointer to input samples
        @param levelOutput  Pointer to level estimates (may already contain values from other channels)
        @param numSamples   Number of samples to process
        @param state        State for this channel
        @param linked       If true, take max of current estimate and existing levelOutput value
    */
    void estimateLevelLinked(const float* input,
                             float* levelOutput,
                             size_t numSamples,
                             State& state,
                             bool linked) const
    {
        for (size_t i = 0; i < numSamples; ++i)
        {
            float level = std::fabs(input[i]);

            if (level > state.levelEstimate)
            {
                // Attack phase
                state.levelEstimate += (level - state.levelEstimate) * attackCoefficient_;
                state.holdCount = 0;
            }
            else if (state.holdCount < holdTimeSamples_)
            {
                // Hold phase
                state.holdCount++;
            }
            else
            {
                // Release phase
                state.levelEstimate += (level - state.levelEstimate) * releaseCoefficient_;
            }

            level = state.levelEstimate;

            if (linked)
            {
                // Take max of this channel's level and existing level
                if (level > levelOutput[i])
                    levelOutput[i] = level;
            }
            else
            {
                levelOutput[i] = level;
            }
        }
    }

private:
    void updateCoefficients()
    {
        attackCoefficient_ = timeConstantToCoefficient(attackTimeSec_, sampleRateHz_);
        releaseCoefficient_ = timeConstantToCoefficient(releaseTimeSec_, sampleRateHz_);
        holdTimeSamples_ = static_cast<size_t>(holdTimeSec_ * sampleRateHz_);
    }

    float sampleRateHz_ = 44100.0f;
    float attackTimeSec_ = 0.0f;
    float releaseTimeSec_ = 0.1f;  // 100ms default
    float holdTimeSec_ = 0.0f;

    float attackCoefficient_ = 1.0f;
    float releaseCoefficient_ = 1.0f;
    size_t holdTimeSamples_ = 0;
};

} // namespace DSP
