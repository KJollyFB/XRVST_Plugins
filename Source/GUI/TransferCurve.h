#pragma once

#include <JuceHeader.h>
#include "../DSP/CompressionCurve.h"
#include "../DSP/Utilities.h"

/**
    Transfer curve visualization for the compressor.

    Shows the input/output relationship graphically, with:
    - Grid lines for reference
    - The compression curve
    - Dynamic cursor showing current operating point
    - Threshold and knee indicators
*/
class TransferCurve : public juce::Component, public juce::Timer
{
public:
    TransferCurve()
    {
        startTimerHz(30);  // 30 FPS update rate
    }

    ~TransferCurve() override
    {
        stopTimer();
    }

    //==============================================================================
    // Configuration

    void setRange(float minDb, float maxDb)
    {
        minDb_ = minDb;
        maxDb_ = maxDb;
        updateCurvePath();
    }

    void setCompressionParams(float thresholdDb, float ratio, float kneeWidthDb)
    {
        compressionCurve_.setThresholdDb(thresholdDb);
        compressionCurve_.setCompressionRatio(ratio);
        compressionCurve_.setKneeWidthDb(kneeWidthDb);
        updateCurvePath();
    }

    void setCurrentInputLevel(float inputLevelDb)
    {
        currentInputLevel_ = inputLevelDb;
    }

    void setColors(juce::Colour background, juce::Colour grid, juce::Colour curve, juce::Colour cursor)
    {
        backgroundColor_ = background;
        gridColor_ = grid;
        curveColor_ = curve;
        cursorColor_ = cursor;
        repaint();
    }

    //==============================================================================
    // Component overrides

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(4.0f);

        // Background
        g.setColour(backgroundColor_);
        g.fillRoundedRectangle(bounds, 4.0f);

        // Border
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

        // Draw grid
        drawGrid(g, bounds);

        // Draw unity line (no compression reference)
        drawUnityLine(g, bounds);

        // Draw compression curve
        drawCompressionCurve(g, bounds);

        // Draw threshold indicator
        drawThresholdIndicator(g, bounds);

        // Draw current operating point
        drawOperatingPoint(g, bounds);

        // Draw axis labels
        drawLabels(g, bounds);
    }

    void resized() override
    {
        updateCurvePath();
    }

    void timerCallback() override
    {
        repaint();
    }

private:
    //==============================================================================
    void drawGrid(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        g.setColour(gridColor_);

        float range = maxDb_ - minDb_;

        // Vertical grid lines (input levels)
        for (float db = minDb_; db <= maxDb_; db += 12.0f)
        {
            float x = bounds.getX() + bounds.getWidth() * (db - minDb_) / range;
            g.drawLine(x, bounds.getY(), x, bounds.getBottom(), 0.5f);
        }

        // Horizontal grid lines (output levels)
        for (float db = minDb_; db <= maxDb_; db += 12.0f)
        {
            float y = bounds.getBottom() - bounds.getHeight() * (db - minDb_) / range;
            g.drawLine(bounds.getX(), y, bounds.getRight(), y, 0.5f);
        }
    }

    void drawUnityLine(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        g.setColour(gridColor_.withAlpha(0.5f));

        // Unity line: input = output (diagonal)
        g.drawLine(bounds.getX(), bounds.getBottom(),
                   bounds.getRight(), bounds.getY(), 1.0f);
    }

    void drawCompressionCurve(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        // Generate curve path
        juce::Path curvePath;
        float range = maxDb_ - minDb_;
        bool first = true;

        for (float inputDb = minDb_; inputDb <= maxDb_; inputDb += 0.5f)
        {
            float outputDb = compressionCurve_.calculateOutputLevelDb(inputDb);

            float x = bounds.getX() + bounds.getWidth() * (inputDb - minDb_) / range;
            float y = bounds.getBottom() - bounds.getHeight() * (outputDb - minDb_) / range;

            // Clamp y to bounds
            y = juce::jlimit(bounds.getY(), bounds.getBottom(), y);

            if (first)
            {
                curvePath.startNewSubPath(x, y);
                first = false;
            }
            else
            {
                curvePath.lineTo(x, y);
            }
        }

        // Draw curve with glow effect
        g.setColour(curveColor_.withAlpha(0.3f));
        g.strokePath(curvePath, juce::PathStrokeType(4.0f));

        g.setColour(curveColor_);
        g.strokePath(curvePath, juce::PathStrokeType(2.0f));
    }

    void drawThresholdIndicator(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        float thresholdDb = compressionCurve_.getThresholdDb();
        float kneeWidthDb = compressionCurve_.getKneeWidthDb();
        float range = maxDb_ - minDb_;

        // Threshold point
        float thresholdX = bounds.getX() + bounds.getWidth() * (thresholdDb - minDb_) / range;
        float thresholdY = bounds.getBottom() - bounds.getHeight() * (thresholdDb - minDb_) / range;

        // Draw knee region
        if (kneeWidthDb > 0.0f)
        {
            float kneeStartDb = thresholdDb - kneeWidthDb / 2.0f;
            float kneeEndDb = thresholdDb + kneeWidthDb / 2.0f;

            float kneeStartX = bounds.getX() + bounds.getWidth() * (kneeStartDb - minDb_) / range;
            float kneeEndX = bounds.getX() + bounds.getWidth() * (kneeEndDb - minDb_) / range;

            g.setColour(curveColor_.withAlpha(0.1f));
            g.fillRect(kneeStartX, bounds.getY(), kneeEndX - kneeStartX, bounds.getHeight());
        }

        // Draw threshold point marker
        g.setColour(curveColor_);
        g.fillEllipse(thresholdX - 4.0f, thresholdY - 4.0f, 8.0f, 8.0f);
    }

    void drawOperatingPoint(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        if (currentInputLevel_ <= minDb_)
            return;

        float range = maxDb_ - minDb_;
        float inputDb = juce::jlimit(minDb_, maxDb_, currentInputLevel_);
        float outputDb = compressionCurve_.calculateOutputLevelDb(inputDb);

        float x = bounds.getX() + bounds.getWidth() * (inputDb - minDb_) / range;
        float y = bounds.getBottom() - bounds.getHeight() * (outputDb - minDb_) / range;

        // Clamp to bounds
        y = juce::jlimit(bounds.getY(), bounds.getBottom(), y);

        // Draw cursor with glow
        g.setColour(cursorColor_.withAlpha(0.5f));
        g.fillEllipse(x - 8.0f, y - 8.0f, 16.0f, 16.0f);

        g.setColour(cursorColor_);
        g.fillEllipse(x - 5.0f, y - 5.0f, 10.0f, 10.0f);

        // Draw crosshairs
        g.setColour(cursorColor_.withAlpha(0.3f));
        g.drawLine(x, bounds.getY(), x, bounds.getBottom(), 0.5f);
        g.drawLine(bounds.getX(), y, bounds.getRight(), y, 0.5f);
    }

    void drawLabels(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        g.setColour(juce::Colours::white.withAlpha(0.6f));
        g.setFont(10.0f);

        // Input label (bottom)
        g.drawText("Input (dB)", bounds.getX(), bounds.getBottom() + 2,
                   bounds.getWidth(), 12, juce::Justification::centred);

        // Output label (left, rotated would be better but this is simpler)
        g.saveState();
        g.addTransform(juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi,
                                                        bounds.getX() - 12.0f,
                                                        bounds.getCentreY()));
        g.drawText("Output (dB)", bounds.getX() - 50.0f, bounds.getCentreY() - 6.0f,
                   100.0f, 12, juce::Justification::centred);
        g.restoreState();

        // dB markers
        float range = maxDb_ - minDb_;

        // X-axis markers
        for (float db = minDb_; db <= maxDb_; db += 24.0f)
        {
            float x = bounds.getX() + bounds.getWidth() * (db - minDb_) / range;
            g.drawText(juce::String(static_cast<int>(db)),
                       x - 15.0f, bounds.getBottom() + 12.0f, 30.0f, 12,
                       juce::Justification::centred);
        }
    }

    void updateCurvePath()
    {
        repaint();
    }

    //==============================================================================
    DSP::CompressionCurve compressionCurve_;

    float minDb_ = -60.0f;
    float maxDb_ = 0.0f;
    float currentInputLevel_ = -100.0f;

    juce::Colour backgroundColor_{ 0xff0f0f23 };
    juce::Colour gridColor_{ 0xff2a2a4a };
    juce::Colour curveColor_{ 0xff00d4ff };
    juce::Colour cursorColor_{ 0xffffb800 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransferCurve)
};
