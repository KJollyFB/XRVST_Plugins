#pragma once

#include <cmath>
#include <limits>

namespace DSP
{

//==============================================================================
// Infinity helpers
//==============================================================================

template <typename T>
constexpr T positiveInfinity()
{
    return std::numeric_limits<T>::has_infinity
               ? std::numeric_limits<T>::infinity()
               : std::numeric_limits<T>::max();
}

template <typename T>
constexpr T negativeInfinity()
{
    return std::numeric_limits<T>::has_infinity
               ? -std::numeric_limits<T>::infinity()
               : std::numeric_limits<T>::lowest();
}

//==============================================================================
// dB / Magnitude conversions
//==============================================================================

/** Convert decibels to linear magnitude.
    @param db The value in decibels
    @return The linear magnitude (voltage ratio)
*/
inline float db2Mag(float db)
{
    if (db == negativeInfinity<float>())
        return 0.0f;
    else if (db == positiveInfinity<float>())
        return positiveInfinity<float>();
    else
        return std::pow(10.0f, db / 20.0f);
}

/** Convert linear magnitude to decibels.
    @param mag The linear magnitude (voltage ratio)
    @return The value in decibels
*/
inline float mag2Db(float mag)
{
    if (mag < 0.0f)
        mag = std::fabs(mag);

    if (mag == 0.0f)
        return negativeInfinity<float>();
    else if (mag == positiveInfinity<float>())
        return positiveInfinity<float>();
    else
        return 20.0f * std::log10(mag);
}

//==============================================================================
// Time constant conversions
//==============================================================================

/** Convert a time constant (in seconds) to a one-pole filter coefficient.

    The coefficient can be used in: y[n] = y[n-1] + coeff * (x[n] - y[n-1])

    @param timeConstantSec The time constant in seconds (time to reach ~63% of target)
    @param sampleRateHz The sample rate in Hz
    @return The filter coefficient (0.0 to 1.0)
*/
inline float timeConstantToCoefficient(float timeConstantSec, float sampleRateHz)
{
    if (sampleRateHz <= 0.0f)
        sampleRateHz = std::fabs(sampleRateHz);

    if (timeConstantSec < 0.0f)
        timeConstantSec = std::fabs(timeConstantSec);

    if (timeConstantSec <= 0.0f)
        return 1.0f;  // Instant response
    else if (sampleRateHz == 0.0f || timeConstantSec == positiveInfinity<float>())
        return 0.0f;  // No response
    else
        return 1.0f - std::exp(-1.0f / (timeConstantSec * sampleRateHz));
}

/** Convert a one-pole filter coefficient to a time constant (in seconds).

    @param coeff The filter coefficient (0.0 to 1.0)
    @param sampleRateHz The sample rate in Hz
    @return The time constant in seconds
*/
inline float coefficientToTimeConstant(float coeff, float sampleRateHz)
{
    if (sampleRateHz <= 0.0f)
        sampleRateHz = std::fabs(sampleRateHz);

    if (sampleRateHz == 0.0f)
        return coeff <= 0.0f ? positiveInfinity<float>() : 0.0f;

    if (coeff <= 0.0f)
        return positiveInfinity<float>();
    else if (coeff >= 1.0f)
        return 0.0f;
    else
        return -1.0f / (std::log(1.0f - coeff) * sampleRateHz);
}

} // namespace DSP
