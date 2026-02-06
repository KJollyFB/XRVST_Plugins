#pragma once

#include <JuceHeader.h>

/**
    VU-style meter for displaying gain reduction.

    Features:
    - Vertical or horizontal orientation
    - Peak hold indicator
    - Customizable colors
    - Smooth animation
*/
class GainReductionMeter : public juce::Component, public juce::Timer
{
public:
    enum class Orientation
    {
        Vertical,
        Horizontal
    };

    GainReductionMeter()
    {
        startTimerHz(30);  // 30 FPS update rate
    }

    ~GainReductionMeter() override
    {
        stopTimer();
    }

    //==============================================================================
    // Configuration

    void setOrientation(Orientation orientation)
    {
        orientation_ = orientation;
        repaint();
    }

    void setRange(float minDb, float maxDb)
    {
        minDb_ = minDb;
        maxDb_ = maxDb;
        repaint();
    }

    void setColors(juce::Colour background, juce::Colour meter, juce::Colour peak)
    {
        backgroundColor_ = background;
        meterColor_ = meter;
        peakColor_ = peak;
        repaint();
    }

    //==============================================================================
    // Update value

    void setGainReductionDb(float gainReductionDb)
    {
        // Gain reduction is negative (e.g., -10 dB means 10 dB of reduction)
        targetLevel_ = gainReductionDb;

        // Update peak hold
        if (gainReductionDb < peakHoldLevel_)
        {
            peakHoldLevel_ = gainReductionDb;
            peakHoldTimer_ = peakHoldTime_;
        }
    }

    float getGainReductionDb() const { return currentLevel_; }

    //==============================================================================
    // Component overrides

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);

        // Background
        g.setColour(backgroundColor_);
        g.fillRoundedRectangle(bounds, 3.0f);

        // Border
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

        // Calculate meter level (0 to 1, where 0 = no reduction, 1 = full reduction)
        float normalizedLevel = juce::jlimit(0.0f, 1.0f, -currentLevel_ / (maxDb_ - minDb_));

        // Draw meter
        if (normalizedLevel > 0.0f)
        {
            juce::Rectangle<float> meterBounds;

            if (orientation_ == Orientation::Vertical)
            {
                float meterHeight = bounds.getHeight() * normalizedLevel;
                meterBounds = bounds.removeFromTop(meterHeight);
            }
            else
            {
                float meterWidth = bounds.getWidth() * normalizedLevel;
                meterBounds = bounds.removeFromLeft(meterWidth);
            }

            // Gradient for meter
            juce::ColourGradient gradient;
            if (orientation_ == Orientation::Vertical)
            {
                gradient = juce::ColourGradient(
                    meterColor_.brighter(0.2f), meterBounds.getX(), meterBounds.getY(),
                    meterColor_.darker(0.3f), meterBounds.getRight(), meterBounds.getY(),
                    false);
            }
            else
            {
                gradient = juce::ColourGradient(
                    meterColor_.brighter(0.2f), meterBounds.getX(), meterBounds.getY(),
                    meterColor_.darker(0.3f), meterBounds.getX(), meterBounds.getBottom(),
                    false);
            }

            g.setGradientFill(gradient);
            g.fillRoundedRectangle(meterBounds, 2.0f);
        }

        // Draw peak hold indicator
        if (peakHoldLevel_ < 0.0f && showPeakHold_)
        {
            float normalizedPeak = juce::jlimit(0.0f, 1.0f, -peakHoldLevel_ / (maxDb_ - minDb_));

            g.setColour(peakColor_);

            auto fullBounds = getLocalBounds().toFloat().reduced(2.0f);

            if (orientation_ == Orientation::Vertical)
            {
                float peakY = fullBounds.getY() + fullBounds.getHeight() * normalizedPeak;
                g.fillRect(fullBounds.getX(), peakY - 1.0f, fullBounds.getWidth(), 2.0f);
            }
            else
            {
                float peakX = fullBounds.getX() + fullBounds.getWidth() * normalizedPeak;
                g.fillRect(peakX - 1.0f, fullBounds.getY(), 2.0f, fullBounds.getHeight());
            }
        }

        // Draw scale marks
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.setFont(10.0f);

        auto fullBounds = getLocalBounds().toFloat().reduced(2.0f);
        float range = maxDb_ - minDb_;

        for (float db = 0.0f; db >= minDb_; db -= 6.0f)
        {
            float normalized = -db / range;

            if (orientation_ == Orientation::Vertical)
            {
                float y = fullBounds.getY() + fullBounds.getHeight() * normalized;
                g.drawHorizontalLine(static_cast<int>(y), fullBounds.getX(), fullBounds.getX() + 5.0f);
            }
        }
    }

    void resized() override
    {
    }

    //==============================================================================
    // Timer callback for smooth animation

    void timerCallback() override
    {
        // Smooth interpolation toward target level
        float smoothingFactor = 0.3f;
        currentLevel_ += (targetLevel_ - currentLevel_) * smoothingFactor;

        // Decay peak hold
        if (peakHoldTimer_ > 0)
        {
            peakHoldTimer_--;
        }
        else
        {
            // Slowly decay peak back to 0
            peakHoldLevel_ += 0.5f;  // 0.5 dB per frame
            if (peakHoldLevel_ > 0.0f)
                peakHoldLevel_ = 0.0f;
        }

        repaint();
    }

private:
    Orientation orientation_ = Orientation::Vertical;

    float minDb_ = -30.0f;
    float maxDb_ = 0.0f;

    float currentLevel_ = 0.0f;
    float targetLevel_ = 0.0f;

    float peakHoldLevel_ = 0.0f;
    int peakHoldTimer_ = 0;
    int peakHoldTime_ = 60;  // ~2 seconds at 30 FPS
    bool showPeakHold_ = true;

    juce::Colour backgroundColor_{ 0xff1a1a2e };
    juce::Colour meterColor_{ 0xff00d4ff };
    juce::Colour peakColor_{ 0xffff6b6b };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainReductionMeter)
};
