#pragma once

#include <JuceHeader.h>

/**
    Custom LookAndFeel for Broadband Compressor V2.

    Provides a modern dark theme with:
    - Dark backgrounds with subtle gradients
    - Cyan/blue accent colors
    - Custom rotary sliders
    - Styled toggle buttons
*/
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        // Set color scheme
        setColour(juce::ResizableWindow::backgroundColourId, backgroundColor_);
        setColour(juce::Slider::backgroundColourId, sliderBackground_);
        setColour(juce::Slider::trackColourId, accentColor_);
        setColour(juce::Slider::thumbColourId, accentColor_);
        setColour(juce::Slider::rotarySliderFillColourId, accentColor_);
        setColour(juce::Slider::rotarySliderOutlineColourId, sliderBackground_);
        setColour(juce::Label::textColourId, textColor_);
        setColour(juce::ToggleButton::textColourId, textColor_);
        setColour(juce::ToggleButton::tickColourId, accentColor_);
        setColour(juce::ToggleButton::tickDisabledColourId, textColor_.darker(0.5f));
    }

    //==============================================================================
    // Rotary Slider

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Background circle
        g.setColour(sliderBackground_);
        g.fillEllipse(rx, ry, rw, rw);

        // Outer ring
        g.setColour(juce::Colours::white.withAlpha(0.1f));
        g.drawEllipse(rx, ry, rw, rw, 2.0f);

        // Arc track (background)
        juce::Path arcBackground;
        arcBackground.addCentredArc(centreX, centreY, radius - 6.0f, radius - 6.0f,
                                     0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(sliderBackground_.brighter(0.2f));
        g.strokePath(arcBackground, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved,
                                                          juce::PathStrokeType::rounded));

        // Arc track (filled)
        juce::Path arcFilled;
        arcFilled.addCentredArc(centreX, centreY, radius - 6.0f, radius - 6.0f,
                                 0.0f, rotaryStartAngle, angle, true);
        g.setColour(accentColor_);
        g.strokePath(arcFilled, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved,
                                                      juce::PathStrokeType::rounded));

        // Pointer
        juce::Path pointer;
        auto pointerLength = radius * 0.5f;
        auto pointerThickness = 3.0f;
        pointer.addRoundedRectangle(-pointerThickness * 0.5f, -radius + 8.0f,
                                     pointerThickness, pointerLength, 1.5f);
        pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.setColour(accentColor_);
        g.fillPath(pointer);

        // Center dot
        g.setColour(accentColor_.brighter(0.3f));
        g.fillEllipse(centreX - 4.0f, centreY - 4.0f, 8.0f, 8.0f);
    }

    //==============================================================================
    // Linear Slider

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        bool isHorizontal = style == juce::Slider::LinearHorizontal ||
                            style == juce::Slider::LinearBar;

        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
        auto trackHeight = 6.0f;

        if (isHorizontal)
        {
            // Track background
            auto trackBounds = bounds.withSizeKeepingCentre(bounds.getWidth() - 8.0f, trackHeight);
            g.setColour(sliderBackground_);
            g.fillRoundedRectangle(trackBounds, trackHeight / 2.0f);

            // Track fill
            auto fillBounds = trackBounds.withWidth(sliderPos - trackBounds.getX());
            g.setColour(accentColor_);
            g.fillRoundedRectangle(fillBounds, trackHeight / 2.0f);

            // Thumb
            auto thumbSize = 16.0f;
            g.setColour(accentColor_);
            g.fillEllipse(sliderPos - thumbSize / 2.0f,
                          bounds.getCentreY() - thumbSize / 2.0f,
                          thumbSize, thumbSize);

            g.setColour(accentColor_.brighter(0.3f));
            g.fillEllipse(sliderPos - 4.0f, bounds.getCentreY() - 4.0f, 8.0f, 8.0f);
        }
        else
        {
            // Vertical slider
            auto trackBounds = bounds.withSizeKeepingCentre(trackHeight, bounds.getHeight() - 8.0f);
            g.setColour(sliderBackground_);
            g.fillRoundedRectangle(trackBounds, trackHeight / 2.0f);

            auto fillBounds = trackBounds.withTop(sliderPos);
            g.setColour(accentColor_);
            g.fillRoundedRectangle(fillBounds, trackHeight / 2.0f);

            auto thumbSize = 16.0f;
            g.setColour(accentColor_);
            g.fillEllipse(bounds.getCentreX() - thumbSize / 2.0f,
                          sliderPos - thumbSize / 2.0f,
                          thumbSize, thumbSize);
        }
    }

    //==============================================================================
    // Toggle Button

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto tickBounds = bounds.removeFromLeft(bounds.getHeight()).reduced(4.0f);

        // Background
        g.setColour(sliderBackground_);
        g.fillRoundedRectangle(tickBounds, 4.0f);

        // Border
        g.setColour(shouldDrawButtonAsHighlighted ? accentColor_ : juce::Colours::white.withAlpha(0.2f));
        g.drawRoundedRectangle(tickBounds, 4.0f, 1.0f);

        // Tick
        if (button.getToggleState())
        {
            auto tickPath = juce::Path();
            auto tickArea = tickBounds.reduced(tickBounds.getWidth() * 0.25f);
            tickPath.startNewSubPath(tickArea.getX(), tickArea.getCentreY());
            tickPath.lineTo(tickArea.getCentreX(), tickArea.getBottom());
            tickPath.lineTo(tickArea.getRight(), tickArea.getY());

            g.setColour(accentColor_);
            g.strokePath(tickPath, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved,
                                                         juce::PathStrokeType::rounded));
        }

        // Label
        g.setColour(textColor_);
        g.setFont(14.0f);
        g.drawText(button.getButtonText(), bounds.reduced(4.0f, 0.0f), juce::Justification::centredLeft);
    }

    //==============================================================================
    // Label

    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.setColour(label.findColour(juce::Label::textColourId));
        g.setFont(getLabelFont(label));

        auto textArea = label.getLocalBounds().toFloat();
        g.drawFittedText(label.getText(), textArea.toNearestInt(),
                         label.getJustificationType(),
                         juce::jmax(1, static_cast<int>(textArea.getHeight() / 12.0f)),
                         label.getMinimumHorizontalScale());
    }

    //==============================================================================
    // Colors

    juce::Colour getBackgroundColor() const { return backgroundColor_; }
    juce::Colour getAccentColor() const { return accentColor_; }
    juce::Colour getTextColor() const { return textColor_; }

private:
    juce::Colour backgroundColor_{ 0xff16162b };
    juce::Colour sliderBackground_{ 0xff1f1f3d };
    juce::Colour accentColor_{ 0xff00d4ff };
    juce::Colour textColor_{ 0xffe0e0e0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};
