#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BroadbandCompressorAudioProcessorEditor::BroadbandCompressorAudioProcessorEditor(
    BroadbandCompressorAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // Set custom look and feel
    setLookAndFeel(&customLookAndFeel_);

    // Add visualization components
    addAndMakeVisible(transferCurve_);
    addAndMakeVisible(gainReductionMeter_);

    // Setup title
    titleLabel_.setText("Broadband Compressor V2", juce::dontSendNotification);
    titleLabel_.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel_.setColour(juce::Label::textColourId, customLookAndFeel_.getAccentColor());
    titleLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel_);

    // Setup GR meter label
    grLabel_.setText("GR", juce::dontSendNotification);
    grLabel_.setFont(juce::Font(12.0f));
    grLabel_.setColour(juce::Label::textColourId, customLookAndFeel_.getTextColor());
    grLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(grLabel_);

    // Setup rotary knobs (main compression parameters)
    setupSlider(inputGainSlider_, " dB", true);
    setupSlider(thresholdSlider_, " dB", true);
    setupSlider(ratioSlider_, ":1", true);
    setupSlider(kneeWidthSlider_, " dB", true);
    setupSlider(outputGainSlider_, " dB", true);

    // Setup horizontal sliders (time parameters)
    setupSlider(attackTimeSlider_, " ms", false);
    setupSlider(releaseTimeSlider_, " ms", false);
    setupSlider(holdTimeSlider_, " ms", false);
    setupSlider(lookaheadTimeSlider_, " ms", false);

    // Setup labels
    setupLabel(inputGainLabel_, "Input Gain");
    setupLabel(thresholdLabel_, "Threshold");
    setupLabel(ratioLabel_, "Ratio");
    setupLabel(kneeWidthLabel_, "Knee");
    setupLabel(outputGainLabel_, "Output Gain");
    setupLabel(attackTimeLabel_, "Attack");
    setupLabel(releaseTimeLabel_, "Release");
    setupLabel(holdTimeLabel_, "Hold");
    setupLabel(lookaheadTimeLabel_, "Lookahead");

    // Setup toggle buttons
    linkedButton_.setColour(juce::ToggleButton::textColourId, customLookAndFeel_.getTextColor());
    bypassButton_.setColour(juce::ToggleButton::textColourId, customLookAndFeel_.getTextColor());
    addAndMakeVisible(linkedButton_);
    addAndMakeVisible(bypassButton_);

    // Create parameter attachments
    auto& vts = processorRef.getValueTreeState();

    inputGainAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        vts, Parameters::ID::inputGain, inputGainSlider_);
    thresholdAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        vts, Parameters::ID::threshold, thresholdSlider_);
    ratioAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        vts, Parameters::ID::ratio, ratioSlider_);
    kneeWidthAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        vts, Parameters::ID::kneeWidth, kneeWidthSlider_);
    outputGainAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        vts, Parameters::ID::outputGain, outputGainSlider_);
    attackTimeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        vts, Parameters::ID::attackTime, attackTimeSlider_);
    releaseTimeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        vts, Parameters::ID::releaseTime, releaseTimeSlider_);
    holdTimeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        vts, Parameters::ID::holdTime, holdTimeSlider_);
    lookaheadTimeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        vts, Parameters::ID::lookaheadTime, lookaheadTimeSlider_);
    linkedAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        vts, Parameters::ID::linked, linkedButton_);
    bypassAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        vts, Parameters::ID::bypass, bypassButton_);

    // Start timer for GUI updates
    startTimerHz(30);

    // Set size
    setSize(800, 500);
}

BroadbandCompressorAudioProcessorEditor::~BroadbandCompressorAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

//==============================================================================
void BroadbandCompressorAudioProcessorEditor::setupSlider(juce::Slider& slider,
                                                             const juce::String& suffix,
                                                             bool isRotary)
{
    if (isRotary)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    }
    else
    {
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    }

    slider.setTextValueSuffix(suffix);
    slider.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(slider);
}

void BroadbandCompressorAudioProcessorEditor::setupLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(12.0f));
    label.setColour(juce::Label::textColourId, customLookAndFeel_.getTextColor());
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

//==============================================================================
void BroadbandCompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(customLookAndFeel_.getBackgroundColor());

    // Section dividers
    g.setColour(juce::Colours::white.withAlpha(0.1f));

    auto bounds = getLocalBounds().reduced(10);
    auto leftSection = bounds.removeFromLeft(220);
    auto rightSection = bounds.removeFromRight(60);

    g.drawVerticalLine(leftSection.getRight() + 5, bounds.getY() + 40.0f, bounds.getBottom() - 10.0f);
    g.drawVerticalLine(rightSection.getX() - 5, bounds.getY() + 40.0f, bounds.getBottom() - 10.0f);
}

void BroadbandCompressorAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Title
    auto titleArea = bounds.removeFromTop(40);
    titleLabel_.setBounds(titleArea);

    // Left section: Transfer curve
    auto leftSection = bounds.removeFromLeft(220);
    transferCurve_.setBounds(leftSection.reduced(5));

    // Right section: Gain reduction meter
    auto rightSection = bounds.removeFromRight(60);
    grLabel_.setBounds(rightSection.removeFromTop(20));
    gainReductionMeter_.setBounds(rightSection.reduced(10, 5));

    // Center section: Controls
    auto centerSection = bounds.reduced(15, 0);

    // Top row: Main knobs
    auto knobRow = centerSection.removeFromTop(130);
    int knobWidth = knobRow.getWidth() / 5;

    auto inputGainArea = knobRow.removeFromLeft(knobWidth);
    inputGainLabel_.setBounds(inputGainArea.removeFromTop(20));
    inputGainSlider_.setBounds(inputGainArea);

    auto thresholdArea = knobRow.removeFromLeft(knobWidth);
    thresholdLabel_.setBounds(thresholdArea.removeFromTop(20));
    thresholdSlider_.setBounds(thresholdArea);

    auto ratioArea = knobRow.removeFromLeft(knobWidth);
    ratioLabel_.setBounds(ratioArea.removeFromTop(20));
    ratioSlider_.setBounds(ratioArea);

    auto kneeArea = knobRow.removeFromLeft(knobWidth);
    kneeWidthLabel_.setBounds(kneeArea.removeFromTop(20));
    kneeWidthSlider_.setBounds(kneeArea);

    auto outputGainArea = knobRow;
    outputGainLabel_.setBounds(outputGainArea.removeFromTop(20));
    outputGainSlider_.setBounds(outputGainArea);

    // Spacer
    centerSection.removeFromTop(20);

    // Bottom section: Time sliders and toggles
    auto sliderHeight = 35;
    auto labelWidth = 70;

    auto attackRow = centerSection.removeFromTop(sliderHeight);
    attackTimeLabel_.setBounds(attackRow.removeFromLeft(labelWidth));
    attackTimeSlider_.setBounds(attackRow);

    auto releaseRow = centerSection.removeFromTop(sliderHeight);
    releaseTimeLabel_.setBounds(releaseRow.removeFromLeft(labelWidth));
    releaseTimeSlider_.setBounds(releaseRow);

    auto holdRow = centerSection.removeFromTop(sliderHeight);
    holdTimeLabel_.setBounds(holdRow.removeFromLeft(labelWidth));
    holdTimeSlider_.setBounds(holdRow);

    auto lookaheadRow = centerSection.removeFromTop(sliderHeight);
    lookaheadTimeLabel_.setBounds(lookaheadRow.removeFromLeft(labelWidth));
    lookaheadTimeSlider_.setBounds(lookaheadRow);

    // Toggle buttons at bottom
    centerSection.removeFromTop(10);
    auto buttonRow = centerSection.removeFromTop(30);
    auto buttonWidth = buttonRow.getWidth() / 3;
    linkedButton_.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(10, 0));
    bypassButton_.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(10, 0));
}

void BroadbandCompressorAudioProcessorEditor::timerCallback()
{
    // Update gain reduction meter
    gainReductionMeter_.setGainReductionDb(processorRef.getGainReductionDb());

    // Update transfer curve visualization
    auto& vts = processorRef.getValueTreeState();
    float threshold = *vts.getRawParameterValue(Parameters::ID::threshold);
    float ratio = *vts.getRawParameterValue(Parameters::ID::ratio);
    float kneeWidth = *vts.getRawParameterValue(Parameters::ID::kneeWidth);

    transferCurve_.setCompressionParams(threshold, ratio, kneeWidth);
    transferCurve_.setCurrentInputLevel(processorRef.getInputLevelDb());
}
