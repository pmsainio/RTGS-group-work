#pragma once 

#include "PluginProcessor.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "mrta_utils/Source/GUI/GenericParameterEditor.h"

class GrainAudioProcessorEditor : public juce::AudioProcessorEditor
{
    public: 
        GrainAudioProcessorEditor(GrainAudioProcessor&);
        ~GrainAudioProcessorEditor();

        void paint(juce::Graphics&) override;
        void resized() override;

        private: 
            GrainAudioProcessor& audioProcessor;
            mrta::GenericParameterEditor paramEditor;
            juce::TextButton loadFileButton {"Load File"};

            JUCE_DECLARE_NON_COPYABLE(GrainAudioProcessorEditor);
}