#pragma once 

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "mrta_utils/Source/GUI/GenericParameterEditor.h"

class GrainAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    public juce::Button::Listener
{
    public: 
        GrainAudioProcessorEditor(GrainAudioProcessor&);
        ~GrainAudioProcessorEditor();

        void paint(juce::Graphics& g) override;
        void resized() override;
        void buttonClicked(juce::Button* button) override;

    private: 
        GrainAudioProcessor& audioProcessor;
        mrta::GenericParameterEditor paramEditor;

        juce::Slider rangeSlider;
        juce::TextButton loadButton;
        std::unique_ptr<juce::FileChooser> fileChooser;

        JUCE_DECLARE_NON_COPYABLE(GrainAudioProcessorEditor);
};