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

    void setGrainAmp(float amp) { grainAmp = amp; }
    void setGrainAttack(float attack) { grainAttack = attack; }
    void setGrainSustain(float sustain) { grainSustain = sustain; }
    void setGrainRelease(float release) { grainRelease = release; }
    void setMinSize(float newMinSize);
    void setMaxSize(float newMaxSize);

    float getGrainAttack() const { return grainAttack; }
    float getGrainSustain() const { return grainSustain; }
    float getGrainRelease() const { return grainRelease; }

    void setFilePosition(float posInSamples) { 
        filePositionInSamples = posInSamples; 
    }
    
    
    DSP::GranSynth* getGranSynth() const { return granSynth; }

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

    float minSize = 480.f;
    float maxSize = 3360.f;

    float grainSize = 480.0f;
    float filePositionInSamples = 0.0f;
};

class GrainSynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};