#pragma once 

#include <JuceHeader.h>
#include "../DSP/GrainEnvelope.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_formats/juce_audio_formats.h"
#include "mrta_utils/Source/Parameter/ParameterManager.h"

namespace Param 
{
    namespace ID
    {

    }

    namespace Name
    {

    }

    namespace Ranges
    {

    }
    
}

class GrainAudioProcessor : public juce::AudioProcessor
{
    public: 
        GrainAudioProcessor();
        ~GrainAudioProcessor();

        // File 
        void readFile(juce::String path);

        void prepareToPlay(double sampleRate, int samplesPerBlock) override;
        void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
        void releaseResources() override;

        void getStateInformation(juce::MemoryBlock& destData) override;
        void setStateInformation(const void* data, int sizeInBytes) override;

        mrta::ParameterManager& getParamManager() { return paramManager; }

        //==============================================================================
        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override;
        const juce::String getName() const override;
        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;
        double getTailLengthSeconds() const override;
        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram(int) override;
        const juce::String getProgramName(int) override;
        void changeProgramName(int, const juce::String&) override;
        //==============================================================================

    private: 
        double sampleRate;
        mrta::ParameterManager paramManager;
        DSP::GrainEnvelope env;
        juce::AudioFormatManager formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainAudioProcessor)
};