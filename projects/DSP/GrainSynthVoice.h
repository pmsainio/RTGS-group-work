#pragma once

#include <JuceHeader.h>
#include "GranSynth.h"

class GrainSynthVoice : public juce::SynthesiserVoice
{
public:
    GrainSynthVoice();
    
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity,
                  juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
    void setSampleBuffer(juce::AudioBuffer<float>* buffer);
    void setGranSynth(DSP::GranSynth* newGranSynth);
    void setGrainParameters(float attackMs, float sustainMs, float releaseMs, float amplitude);

private:
    DSP::GranSynth* granSynth = nullptr;
    juce::AudioBuffer<float>* sampleBuffer = nullptr;
    double sampleRate = 48000.0;
    float pitchRatio = 1.0f;
    float level = 0.0f;
    bool isPlaying = false;
    
    // Grain parameters
    float grainAttack = 5.0f;
    float grainSustain = 50.0f;
    float grainRelease = 20.0f;
    float grainAmp = 1.0f;
};

class GrainSynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};