#pragma once

#include <JuceHeader.h>

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

private:
    double grainPosition = 0.0;
    double pitchRatio = 1.0;
    float level = 0.0f;
    bool isPlaying = false;

    juce::AudioBuffer<float>* sampleBuffer = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainSynthVoice)
};
