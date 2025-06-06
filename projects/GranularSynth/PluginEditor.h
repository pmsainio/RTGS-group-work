#pragma once 

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "mrta_utils/Source/GUI/GenericParameterEditor.h"

class GrainAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    public juce::Button::Listener,
                                    public juce::ChangeListener
{
    public: 
        GrainAudioProcessorEditor(GrainAudioProcessor&);
        ~GrainAudioProcessorEditor();

        void paint(juce::Graphics& g) override;
        void resized() override;
        void buttonClicked(juce::Button* button) override;

        void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    private: 
        GrainAudioProcessor& audioProcessor;
        mrta::GenericParameterEditor paramEditor;

        juce::Slider rangeSlider;
        juce::TextButton loadButton;
        std::unique_ptr<juce::FileChooser> fileChooser;
        
        juce::Path waveformPath;
        bool fileLoaded = false;
        void generateWaveform();

        JUCE_DECLARE_NON_COPYABLE(GrainAudioProcessorEditor);
};