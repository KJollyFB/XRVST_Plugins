#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/GainReductionMeter.h"
#include "GUI/TransferCurve.h"
#include "GUI/CustomLookAndFeel.h"

/**
    GUI Editor for Broadband Compressor V2.

    Layout:
    - Left side: Transfer curve visualization
    - Center: Parameter knobs and sliders
    - Right side: Gain reduction meter
*/
class BroadbandCompressorAudioProcessorEditor : public juce::AudioProcessorEditor,
                                                   public juce::Timer
{
public:
    BroadbandCompressorAudioProcessorEditor(BroadbandCompressorAudioProcessor&);
    ~BroadbandCompressorAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    //==============================================================================
    void setupSlider(juce::Slider& slider, const juce::String& suffix, bool isRotary = true);
    void setupLabel(juce::Label& label, const juce::String& text);

    //==============================================================================
    BroadbandCompressorAudioProcessor& processorRef;

    // Custom look and feel
    CustomLookAndFeel customLookAndFeel_;

    // Visualization components
    TransferCurve transferCurve_;
    GainReductionMeter gainReductionMeter_;

    // Parameter sliders
    juce::Slider inputGainSlider_;
    juce::Slider thresholdSlider_;
    juce::Slider ratioSlider_;
    juce::Slider kneeWidthSlider_;
    juce::Slider outputGainSlider_;
    juce::Slider attackTimeSlider_;
    juce::Slider releaseTimeSlider_;
    juce::Slider holdTimeSlider_;
    juce::Slider lookaheadTimeSlider_;

    // Toggle buttons
    juce::ToggleButton linkedButton_{ "Linked" };
    juce::ToggleButton bypassButton_{ "Bypass" };

    // Labels
    juce::Label inputGainLabel_;
    juce::Label thresholdLabel_;
    juce::Label ratioLabel_;
    juce::Label kneeWidthLabel_;
    juce::Label outputGainLabel_;
    juce::Label attackTimeLabel_;
    juce::Label releaseTimeLabel_;
    juce::Label holdTimeLabel_;
    juce::Label lookaheadTimeLabel_;
    juce::Label titleLabel_;
    juce::Label grLabel_;  // Gain reduction meter label

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kneeWidthAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackTimeAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseTimeAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> holdTimeAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lookaheadTimeAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> linkedAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BroadbandCompressorAudioProcessorEditor)
};
