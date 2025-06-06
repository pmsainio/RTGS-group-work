#include "GranSynth.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include <cstdlib>

namespace DSP {

GranSynth::GranSynth(double sampleRate) : sampleRate(sampleRate)
{
    grains.resize(64);
    for (auto& grain : grains) {
        grain.envelope = std::make_unique<GrainEnvelope>();
    }
}

GranSynth::~GranSynth() = default;

float GranSynth::frandom()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

double GranSynth::randomDuration(float minLength, float maxLength)
{
    return (minLength + frandom() * (maxLength - minLength));
}

void GranSynth::prepare(double newSampleRate)
{
    sampleRate = newSampleRate; 
    for (auto& grain : grains)
    {
        grain.envelope->prepare(sampleRate, 0);
        grain.envelope->setAttackTime(grainAttack);
        grain.envelope->setSustainTime(grainSustain);
        grain.envelope->setReleaseTime(grainRelease);
        grain.envelope->setGrainAmplitude(grainAmplitude);
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
    if (!sampleBuffer || fileLength == 0) {
        DBG("Cannot synthesize - no buffer or empty file");
        return;
    }

    float grainRate = density * 50.0f;  
    float gap = sampleRate / grainRate;
    float currPos = 0.f;
    int grainsTriggered = 0;

    while (currPos < fileLength)
    {
        float grainSize = randomDuration(minSize, maxSize);
        int startPos = static_cast<int>(currPos);
        int endPos = startPos + static_cast<int>(grainSize);
        
        endPos = std::min(endPos, fileLength);
        
        if ((endPos - startPos) < (sampleRate * 0.010f)) 
            break;
    
        if (!trigger(startPos, endPos, pitchRatio))
            break; 

        grainsTriggered++;
        
        currPos += gap;
    }

    DBG("Triggered " << grainsTriggered << " grains");
}

void GranSynth::processBlock(juce::AudioBuffer<float>& outputBuffer)
{
    if (!sampleBuffer || fileLength == 0) return;

    const int numSamples = outputBuffer.getNumSamples();
    const int numChannels = outputBuffer.getNumChannels();
    outputBuffer.clear();

    for (auto& grain : grains)
    {
        if (!grain.active || !grain.envelope) continue;
        int samplesToProcess = numSamples;
        int samplesUntilEnd = grain.endPos - grain.currPos;
        
        if (samplesUntilEnd <= 0)
        {
            grain.active = false;
            continue;
        }
        
        samplesToProcess = std::min(samplesToProcess, samplesUntilEnd);

        // Process envelope
        juce::AudioBuffer<float> envelopeBuffer(1, samplesToProcess);
        float* envelopeData = envelopeBuffer.getWritePointer(0);
        grain.envelope->process(envelopeData, samplesToProcess);

        for (int i = 0; i < samplesToProcess; i++)
        {
            if (grain.currPos >= sampleBuffer->getNumSamples())
            {
                grain.active = false;
                break;
            }

            if (grain.currPos >= grain.endPos || grain.envelope->isOff())
            {
                grain.active = false;
                continue;
            }

            int idx = static_cast<int>(grain.currPosFloat);
            float frac = grain.currPosFloat - idx;
            float s1 = sampleBuffer->getSample(0, idx);
            float s2 = sampleBuffer->getSample(0, std::min(idx + 1, sampleBuffer->getNumSamples() - 1));
            float sample = s1 + frac * (s2 - s1);

            float envValue = envelopeData[i];
            
            // Apply windowing and amplitude
            float windowedSample = sample * envValue * grain.amplitude;
            
            outputBuffer.addSample(0, i, windowedSample);
            if (numChannels > 1)
            {
                outputBuffer.addSample(1, i, windowedSample);
            }
            
            grain.currPosFloat += grain.pitchRatio;  
        }

        if (grain.currPos >= grain.endPos || grain.envelope->isOff())
        {
            grain.active = false;
            DBG("Grain finished at pos: " << grain.currPos);
        }

        if (grain.currPosFloat >= static_cast<float>(grain.endPos) || grain.envelope->isOff())
        {
            grain.active = false;
        }

    }
}

void GranSynth::setGrainEnv(float attack, float sustain, float release)
{
    grainAttack = juce::jlimit(1.0f, 100.0f, attack);
    grainSustain = juce::jlimit(1.0f, 1000.0f, sustain);
    grainRelease = juce::jlimit(1.0f, 100.0f, release);
    
    for (auto& grain : grains)
    {
        if (!grain.active) continue;

        float grainDurationMs = 1000.0f * (grain.endPos - grain.startPos) / sampleRate;

        grain.envelope->setAttackTime(std::min(grainAttack, grainDurationMs * 0.3f));
        grain.envelope->setReleaseTime(std::min(grainRelease, grainDurationMs * 0.3f));
        grain.envelope->setSustainTime(grainDurationMs * 0.4f);
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

bool GranSynth::trigger(int startPos, int endPos, float pitchRatio)
{
    for (auto& grain : grains)
    {
        if (!grain.active)
        {
            grain.startPos = startPos;
            grain.currPosFloat = static_cast<float>(startPos);
            grain.currPos = startPos;
            grain.endPos = endPos;
            grain.pitchRatio = pitchRatio;
            grain.active = true;
            grain.amplitude = grainAmplitude;

            if (grain.envelope)
            {
                grain.envelope->prepare(sampleRate, 0);
                grain.envelope->start();

                float grainDurationMs = 1000.0f * (endPos - startPos) / sampleRate;
                grain.envelope->setAttackTime(std::min(grainAttack, grainDurationMs * 0.3f));
                grain.envelope->setReleaseTime(std::min(grainRelease, grainDurationMs * 0.3f));
                grain.envelope->setSustainTime(grainDurationMs * 0.4f);
            }

            return false;
        }
    }
    
    return true;
}

} // namespace DSP