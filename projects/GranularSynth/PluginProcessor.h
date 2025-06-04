#pragma once 

#include <JuceHeader.h>
#include "../DSP/GrainEnvelope.h"
#include "../DSP/GrainSynthVoice.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_formats/juce_audio_formats.h"

namespace Param 
{
    namespace ID
    {
        static const juce::String volume {"Volume"};
        static const juce::String attack  {"Attack Time"};
        static const juce::String sustain {"Sustain"};
        static const juce::String release {"Release"};
        static const juce::String filePos {"Position"};
        static const juce::String grainLen {"Grain Length"};

    }

    namespace Name
    {
        static const juce::String volume {"Volume"};
        static const juce::String attack  {"Attack Time"};
        static const juce::String sustain {"Sustain"};
        static const juce::String release {"Release"};
        static const juce::String filePos {"Position"};
        static const juce::String grainLen {"Grain Length"};
    }

    namespace Ranges
    {
        static constexpr float volumeMax { 1.f };
        static constexpr float volumeMin { 0.f };
        static constexpr float attackMin  { 5.f };
        static constexpr float attackMax  { 25.f };
        static constexpr float susMin  { 0.01f };
        static constexpr float susMax  { 1.f };
        static constexpr float relMin  { 5.f }; // Add offset
        static constexpr float relMax  { 25.f }; // Add offset
        static constexpr float filePosMin { 0.f };
        static constexpr float filePosMax { 1.f };
        static constexpr float grainSizeMin { 480.f };
        static constexpr float grainSizeMax { 3360.f };
    }
    
}

class GrainAudioProcessor : public juce::AudioProcessor
{
    public: 
        GrainAudioProcessor();
        ~GrainAudioProcessor() override;

        // File 
        void readFile(juce::String path);
        void checkForRestoredPath();
        juce::String restoredPath;
        juce::String chosenPath;

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
        juce::Synthesiser synth;
        GrainSynthVoice* voice { nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainAudioProcessor)
};