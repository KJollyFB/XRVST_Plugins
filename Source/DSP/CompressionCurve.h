#pragma once

#include "Utilities.h"
#include <cmath>

namespace DSP
{

/**
    Soft-knee compression curve calculator.

    This class calculates the gain reduction for a compressor based on
    threshold, ratio, and knee width parameters. It implements a three-region
    soft-knee algorithm:
    - Below knee: no compression (unity gain)
    - In knee: quadratic interpolation for smooth transition
    - Above knee: constant ratio compression
*/
class CompressionCurve
{
public:
    CompressionCurve() = default;

    //==============================================================================
    // Makeup Gain

    void setMakeupGainDb(float makeupGainDb)
    {
        makeupGain_ = db2Mag(makeupGainDb);
    }

    float getMakeupGainDb() const
    {
        return mag2Db(makeupGain_);
    }

    float getMakeupGainLinear() const
    {
        return makeupGain_;
    }

    //==============================================================================
    // Threshold

    void setThresholdDb(float thresholdDb)
    {
        thresholdDb_ = thresholdDb;
    }

    float getThresholdDb() const
    {
        return thresholdDb_;
    }

    //==============================================================================
    // Knee Width

    void setKneeWidthDb(float kneeWidthDb)
    {
        if (kneeWidthDb >= 0.0f)
            kneeWidthDb_ = kneeWidthDb;
    }

    float getKneeWidthDb() const
    {
        return kneeWidthDb_;
    }

    //==============================================================================
    // Compression Ratio

    void setCompressionRatio(float ratio)
    {
        if (ratio >= 1.0f)
            compressionRatio_ = ratio;
    }

    float getCompressionRatio() const
    {
        return compressionRatio_;
    }

    //==============================================================================
    // Gain Calculation

    /** Calculate the compression gain for a given input level.

        @param inLevelDb The input level in dB
        @return The gain to apply in dB (will be negative or zero for compression)
    */
    float calculateCompressionGainDb(float inLevelDb) const
    {
        float outLevelDb;

        if (inLevelDb == negativeInfinity<float>())
        {
            // Silent input, no gain change
            return 0.0f;
        }
        else if (2.0f * (inLevelDb - thresholdDb_) <= -kneeWidthDb_)
        {
            // Below knee - no compression
            // Based on: http://c4dm.eecs.qmul.ac.uk/audioengineering/compressors/documents/Reiss-Tutorialondynamicrangecompression.pdf
            outLevelDb = inLevelDb;
        }
        else if (2.0f * std::fabs(inLevelDb - thresholdDb_) < kneeWidthDb_)
        {
            // In the knee region - quadratic soft knee
            float temp = inLevelDb - thresholdDb_ + kneeWidthDb_ / 2.0f;

            if (compressionRatio_ == positiveInfinity<float>())
            {
                // Brick wall limiting
                outLevelDb = inLevelDb + (-temp * temp / (2.0f * kneeWidthDb_));
            }
            else
            {
                outLevelDb = inLevelDb + (1.0f / compressionRatio_ - 1.0f) * temp * temp / (2.0f * kneeWidthDb_);
            }
        }
        else
        {
            // Above knee - full compression
            if (compressionRatio_ == positiveInfinity<float>())
            {
                // Brick wall limiting
                outLevelDb = thresholdDb_;
            }
            else
            {
                outLevelDb = thresholdDb_ + (inLevelDb - thresholdDb_) / compressionRatio_;
            }
        }

        return outLevelDb - inLevelDb;
    }

    /** Calculate the output level for a given input level (for visualization).

        @param inLevelDb The input level in dB
        @return The output level in dB
    */
    float calculateOutputLevelDb(float inLevelDb) const
    {
        return inLevelDb + calculateCompressionGainDb(inLevelDb);
    }

    /** Apply compression to a buffer of audio samples.

        @param input         Pointer to input samples
        @param levelEstimate Pointer to level estimates (linear magnitude, NOT in dB)
        @param output        Pointer to output samples
        @param numSamples    Number of samples to process
        @param gainReduction Optional pointer to store gain reduction values (in dB, for metering)
    */
    void applyCompression(const float* input,
                          const float* levelEstimate,
                          float* output,
                          size_t numSamples,
                          float* gainReduction = nullptr) const
    {
        for (size_t i = 0; i < numSamples; ++i)
        {
            // Convert level estimate to dB
            float levelDb = mag2Db(levelEstimate[i]);

            // Calculate gain reduction in dB
            float gainDb = calculateCompressionGainDb(levelDb);

            // Convert to linear gain
            float gain = db2Mag(gainDb);

            // Apply compression and makeup gain
            output[i] = input[i] * gain * makeupGain_;

            // Store gain reduction for metering
            if (gainReduction != nullptr)
                gainReduction[i] = gainDb;
        }
    }

    /** Get the current gain reduction for a given level (for real-time metering).

        @param levelLinear The current level estimate (linear magnitude)
        @return The gain reduction in dB (negative value)
    */
    float getGainReductionDb(float levelLinear) const
    {
        float levelDb = mag2Db(levelLinear);
        return calculateCompressionGainDb(levelDb);
    }

private:
    float makeupGain_ = 1.0f;       // Linear makeup gain
    float thresholdDb_ = 0.0f;      // Threshold in dB
    float kneeWidthDb_ = 0.0f;      // Knee width in dB
    float compressionRatio_ = 1.0f; // Compression ratio (1:1 = no compression)
};

} // namespace DSP
