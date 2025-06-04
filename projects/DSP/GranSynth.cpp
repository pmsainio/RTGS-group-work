#include "GranSynth.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include <cstdlib>

namespace DSP {

GranSynth::GranSynth(double sampleRate) 
{
    grains.resize(64);
}

float GranSynth::frandom()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

double GranSynth::randomDuration(float minLength, float maxLength)
{
    return (minLength + frandom() * (maxLength - minLength) * sampleRate);
}

void GranSynth::prepare(double newSampleRate)
{
    sampleRate = newSampleRate; 
    for (auto& grain : grains)
    {
        grain.envelope.prepare(sampleRate, 0);
    }
}

void GranSynth::setBuffer(juce::AudioBuffer<float>* buffer)
{
    sampleBuffer = buffer;
    if (buffer)
    {
        fileLength = buffer->getNumSamples();
    }
}

void GranSynth::synthesize(float density, float minSize, float maxSize)
{
    if (!sampleBuffer || fileLength == 0) { return; }

    for (auto& grain : grains)
    {
        grain.active = false;
    }

    float grainRate = 1.f + density + 49.f;
    float gap = sampleRate / grainRate;

    float currPos = 0.f;

    while (currPos < fileLength)
    {
        float grainSize = minSize + frandom() * (maxSize - minSize);
        int grainSizeSamples = static_cast<int>(grainSize * sampleRate);
        int startPos = static_cast<int>(currPos);
        int endPos = startPos + grainSizeSamples;
        
        if (endPos > fileLength)
            break;
        
        trigger(startPos, endPos);
        currPos += gap * (0.8f + 0.4f * frandom());
    }
}

void GranSynth::processBlock(juce::AudioBuffer<float>& outputBuffer)
{
    if (!sampleBuffer || fileLength == 0) return;

    const int numSamples = outputBuffer.getNumSamples();
    const int numChannels = outputBuffer.getNumChannels();
    outputBuffer.clear();

    for (auto& grain : grains)
    {
        if (!grain.active) continue;

        int samplesRemaining = grain.endPos - grain.currPos;
        int samplesToProcess = std::min(numSamples, samplesRemaining);
        
        if (samplesToProcess <= 0)
        {
            grain.active = false;
            continue;
        }

        grain.envelopeBuffer.setSize(1, samplesToProcess, false, false, true);
        float* envelopeData = grain.envelopeBuffer.getWritePointer(0);
        grain.envelope.process(envelopeData, static_cast<unsigned int>(samplesToProcess));

        for (int i = 0; i < samplesToProcess; i++)
        {
            if (grain.currPos >= fileLength)
            {
                grain.active = false;
                break;
            }

            float sample = sampleBuffer->getSample(0, grain.currPos);
            float envValue = envelopeData[i];
            float gain = envValue * grain.amplitude;
            
            outputBuffer.addSample(0, i, sample * gain);
            if (numChannels > 1)
            {
                outputBuffer.addSample(1, i, sample * gain);
            }
            
            grain.currPos++;
        }

        if (grain.currPos >= grain.endPos)
        {
            grain.active = false;
        }
    }
}

void GranSynth::setGrainEnv(float attack, float release, float sustain)
{
    grainAttack = attack;
    grainRelease = release;
    grainSustain = sustain;

    for (auto& grain : grains)
    {
        grain.envelope.setAttackTime(grainAttack);
        grain.envelope.setReleaseTime(grainRelease);
        grain.envelope.setSustainTime(grainSustain);

    }
}

void GranSynth::setGrainAmp(float amplitude)
{
    grainAmplitude = amplitude;
    for (auto& grain : grains)
    {
        grain.amplitude = grainAmplitude;
    }
}

void GranSynth::trigger(int startPos, int endPos)
{
    for (auto& grain : grains)
    {
        if (!grain.active)
        {
            grain.startPos = startPos;
            grain.endPos = endPos;
            grain.currPos = startPos;
            grain.active = true;
            grain.amplitude = grainAmplitude;
            
            grain.envelope.start();
            break;
        }
    }
}

} // namespace DSP