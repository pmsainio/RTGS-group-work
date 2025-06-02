#pragma once 

#include <JuceHeader.h>
#include "juce_audio_basics/juce_audio_basics.h"
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

class GrainAudioProcessor : public Juce::AudioProcessor
{
    public: 
        GrainAudioProcessor();
        ~GrainAudioProcessor();

        void readFile(juce::AudioSampleBuffer& fileBuffer);
        
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
        mrta::ParameterManager paramManager;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiHandlerAudioProcessor)
};