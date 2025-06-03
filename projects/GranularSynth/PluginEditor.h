#pragma once 

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "mrta_utils/Source/GUI/GenericParameterEditor.h"

class GrainAudioProcessorEditor : public juce::AudioProcessorEditor
{
    public: 
        GrainAudioProcessorEditor(GrainAudioProcessor&);
        ~GrainAudioProcessorEditor();

        void paint(juce::Graphics& g) override;
        void resized() override;

        private: 
            GrainAudioProcessor& audioProcessor;
            mrta::GenericParameterEditor paramEditor;

            JUCE_DECLARE_NON_COPYABLE(GrainAudioProcessorEditor);
};