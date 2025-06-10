#pragma once

#include <JuceHeader.h>
#include "GranSynth.h"

class GrainSynthVoice : public juce::SynthesiserVoice
{
public:
    GrainSynthVoice();
    
    // MIDI Functions. Inherited from the juce Synthesizer class. 
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity,
                  juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
    // Set the buffer, so that a pointer to the fileBuffer exists. 
    // Set the granSynth object, so we can invoke its functions. 
    void setSampleBuffer(juce::AudioBuffer<float>* buffer);
    void setGranSynth(DSP::GranSynth* newGranSynth);
    
    // Set all params so that they get passed to the granSynth class, nothing complicated here... I hope.
    // Removed some functions that are not being used, we can make new ones if required. 
    void setGrainAttack(float attack) { grainAttack = attack; }
    void setGrainSustain(float sustain) { grainSustain = sustain; }
    void setGrainRelease(float release) { grainRelease = release; }
    void setMinSize(float newMinSize);
    void setMaxSize(float newMaxSize);
    void setDensity(float newDensity);
    void setFilePos(float value);
    
    // Set GranSynth pointer to object. Just C++ stuff so we don't keep copying a new object over and over again. 
    // This prevents from using more memory than we need by not creating unecessary objects. 
    DSP::GranSynth* getGranSynth() const { return granSynth; }

private:
    // Initialized to a nullptr, which points to nothing. The reference to the object is given in the function def. 
    DSP::GranSynth* granSynth = nullptr;
    juce::AudioBuffer<float>* sampleBuffer = nullptr;

    // MIDI params. 
    double sampleRate = 48000.0;
    float pitchRatio = 1.0f;
    float level = 0.0f;
    bool isPlaying = false;

    // Grain parameters
    // GRAIN ENV
    float grainAttack = 30.0f;
    float grainSustain = 0.1f;
    float grainRelease = 30.0f;
    float grainAmp = 1.0f;

    bool noteHeld = false;
    // GRAIN SIZE 
    float density = 10.0f;
    float maxGrainSize = 200.0f; // in ms
    float minGrainSize = 50.0f; // in ms
    int filePositionInSamples = 0.0f;

    double timeUntilNextGrain = 0.0;
    double grainIntervalSamples = 0.0;
};

class GrainSynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};