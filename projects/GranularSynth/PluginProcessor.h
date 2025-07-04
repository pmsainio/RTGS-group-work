#pragma once 

#include <JuceHeader.h>
#include "../DSP/GrainSynthVoice.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_formats/juce_audio_formats.h"

// Param Namespace for Knob Registration
namespace Param 
{
    namespace ID
    {
        static const juce::String attack  {"Attack Time"};
        static const juce::String sustain {"Sustain Level"};
        static const juce::String release {"Release Time"};
        static const juce::String filePos {"File Position"};
        static const juce::String grainLen {"Grain Size"};
        static const juce::String density {"Grain Density"};
        static const juce::String minGrainLen {"Min Grain Length"};
        static const juce::String maxGrainLen {"Max Grain Length"};
    }

    namespace Name
    {
        static const juce::String attack  {"Attack Time"};
        static const juce::String sustain {"Sustain Level"};
        static const juce::String release {"Release Time"};
        static const juce::String filePos {"Position"};
        static const juce::String grainLen {"Grain Length"};
        static const juce::String density {"Grain Density"};
        static const juce::String minGrainLen {"Min Grain Length"};
        static const juce::String maxGrainLen {"Max Grain Length"};
    }
    
}

class GrainAudioProcessor : public juce::AudioProcessor, public juce::ChangeBroadcaster
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

        const juce::AudioBuffer<float>* getAudioBuffer() const { return fileBuffer.get(); }
        void notifyWaveformChanged() { sendChangeMessage(); }

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
        
        juce::Synthesiser synth;
        GrainSynthVoice* voice { nullptr };
        std::unique_ptr<DSP::GranSynth> granSynth;
        
        juce::AudioFormatManager formatManager;
        std::unique_ptr<juce::AudioBuffer<float>> fileBuffer;

        juce::ChangeBroadcaster changeBroadcaster;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainAudioProcessor)
};