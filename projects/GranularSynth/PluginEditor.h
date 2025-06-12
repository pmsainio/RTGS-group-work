#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


class GrainAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Button::Listener,
                                   public juce::ChangeListener, 
                                   public juce::Timer
{
public:
    GrainAudioProcessorEditor(GrainAudioProcessor&);
    ~GrainAudioProcessorEditor() override;

    void timerCallback() override;
    void paint(juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    GrainAudioProcessor& audioProcessor;

    std::unique_ptr<mrta::ParameterSlider> grainSizeSlider, filePosSlider, densitySlider;
    std::unique_ptr<mrta::ParameterSlider> attackSlider, sustainSlider, releaseSlider;

    juce::Label grainSizeLabel, filePosLabel, densityLabel;
    juce::Label attackLabel, sustainLabel, releaseLabel;

    //juce::Slider rangeSlider;
    juce::TextButton loadButton;
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::Path waveformPath;
    bool fileLoaded = false;

    void generateWaveform();
    void addLabeledKnob(std::unique_ptr<mrta::ParameterSlider>& slider, juce::Label& label, const juce::String& text, const juce::String& paramID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainAudioProcessorEditor)
};
