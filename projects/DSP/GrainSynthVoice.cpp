#include "GrainSynthVoice.h"

GrainSynthVoice::GrainSynthVoice()
{
}

GrainSynthVoice::~GrainSynthVoice()
{
}

bool GrainSynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void GrainSynthVoice::startNote(int midiNoteNumber, float velocity,
                                juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
{
    pitchRatio = std::pow(2.0, (midiNoteNumber - 60) / 12.0);

    grainPosition = 0.0;
    level = velocity;
    isPlaying = true;
}

void GrainSynthVoice::stopNote(float /*velocity*/, bool /*allowTailOff*/)
{
    isPlaying = false;
    clearCurrentNote();
}

void GrainSynthVoice::pitchWheelMoved(int)
{
}

void GrainSynthVoice::controllerMoved(int, int)
{
}

void GrainSynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isPlaying || sampleBuffer == nullptr)
        return;

    auto numChannels = outputBuffer.getNumChannels();

    for (int i = 0; i < numSamples; ++i)
    {
        int index0 = static_cast<int>(grainPosition) % sampleBuffer->getNumSamples();
        int index1 = (index0 + 1) % sampleBuffer->getNumSamples();
        float frac = static_cast<float>(grainPosition - static_cast<float>(index0));

        float sample = (1.0f - frac) * sampleBuffer->getSample(0, index0)
                        + frac * sampleBuffer->getSample(0, index1);

        for (int ch = 0; ch < numChannels; ++ch)
        {
            outputBuffer.addSample(ch, startSample + i, sample * level);
        }

        grainPosition += pitchRatio;

        if (grainPosition >= sampleBuffer->getNumSamples())
            grainPosition -= sampleBuffer->getNumSamples();
    }
}

void GrainSynthVoice::setSampleBuffer(juce::AudioBuffer<float>* buffer)
{
    sampleBuffer = buffer;
}
