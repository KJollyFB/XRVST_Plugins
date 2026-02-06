#pragma once

#include <vector>
#include <cstddef>
#include <algorithm>

namespace DSP
{

/**
    Simple circular buffer delay line for lookahead functionality.

    Provides sample-accurate delay with variable delay time.
    Maximum delay is set at construction time.
*/
class DelayLine
{
public:
    DelayLine() = default;

    /** Create a delay line with specified maximum delay.
        @param maxDelaySamples Maximum delay in samples
    */
    explicit DelayLine(size_t maxDelaySamples)
        : maxDelay_(maxDelaySamples)
    {
        buffer_.resize(maxDelaySamples + 1, 0.0f);
    }

    /** Prepare the delay line for processing.
        @param maxDelaySamples Maximum delay in samples
    */
    void prepare(size_t maxDelaySamples)
    {
        maxDelay_ = maxDelaySamples;
        buffer_.resize(maxDelaySamples + 1, 0.0f);
        reset();
    }

    /** Clear the delay buffer. */
    void reset()
    {
        std::fill(buffer_.begin(), buffer_.end(), 0.0f);
        writeIndex_ = 0;
    }

    /** Set the delay time in samples.
        @param delaySamples Delay in samples (0 to maxDelay)
    */
    void setDelay(size_t delaySamples)
    {
        delay_ = std::min(delaySamples, maxDelay_);
    }

    /** Set the delay time in milliseconds.
        @param delayMs Delay in milliseconds
        @param sampleRate Current sample rate
    */
    void setDelayMs(float delayMs, float sampleRate)
    {
        size_t delaySamples = static_cast<size_t>(delayMs * sampleRate / 1000.0f);
        setDelay(delaySamples);
    }

    /** Get the current delay in samples. */
    size_t getDelay() const { return delay_; }

    /** Get the maximum delay in samples. */
    size_t getMaxDelay() const { return maxDelay_; }

    /** Process a single sample through the delay line.
        @param input The input sample
        @return The delayed output sample
    */
    float processSample(float input)
    {
        // Write input to buffer
        buffer_[writeIndex_] = input;

        // Calculate read index
        size_t readIndex = (writeIndex_ + buffer_.size() - delay_) % buffer_.size();

        // Read delayed sample
        float output = buffer_[readIndex];

        // Advance write index
        writeIndex_ = (writeIndex_ + 1) % buffer_.size();

        return output;
    }

    /** Process a buffer of samples.
        @param input  Pointer to input samples
        @param output Pointer to output samples
        @param numSamples Number of samples to process
    */
    void process(const float* input, float* output, size_t numSamples)
    {
        for (size_t i = 0; i < numSamples; ++i)
        {
            output[i] = processSample(input[i]);
        }
    }

    /** Process in-place.
        @param buffer Pointer to samples (will be overwritten)
        @param numSamples Number of samples to process
    */
    void processInPlace(float* buffer, size_t numSamples)
    {
        for (size_t i = 0; i < numSamples; ++i)
        {
            buffer[i] = processSample(buffer[i]);
        }
    }

private:
    std::vector<float> buffer_;
    size_t writeIndex_ = 0;
    size_t delay_ = 0;
    size_t maxDelay_ = 0;
};

} // namespace DSP
