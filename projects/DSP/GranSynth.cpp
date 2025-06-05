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
        
        //if ((endPos - startPos) < (sampleRate * 0.010f)) 
        //  break;
        
        trigger(startPos, endPos);
        grainsTriggered++;
        
        currPos += (fileLength - maxSize) * frandom(); 
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

            float sample = sampleBuffer->getSample(0, grain.currPos);
            float envValue = envelopeData[i];
            
            // Apply windowing and amplitude
            float windowedSample = sample * envValue * grain.amplitude;
            
            outputBuffer.addSample(0, i, windowedSample);
            if (numChannels > 1)
            {
                outputBuffer.addSample(1, i, windowedSample);
            }
            
            grain.currPos++;
        }

        if (grain.currPos >= grain.endPos || grain.envelope->isOff())
        {
            grain.active = false;
            DBG("Grain finished at pos: " << grain.currPos);
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
        grain.envelope->setAttackTime(grainAttack);
        grain.envelope->setSustainTime(grainSustain);
        grain.envelope->setReleaseTime(grainRelease);
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
            
            if (grain.envelope) {
                grain.envelope->prepare(sampleRate, 0); // Re-prepare to reset envelope
                grain.envelope->start();
                
                // Calculate envelope times based on grain duration
                float grainDurationMs = 1000.0f * (endPos - startPos) / sampleRate;
                grain.envelope->setAttackTime(juce::jmin(grainAttack, grainDurationMs * 0.3f));
                grain.envelope->setReleaseTime(juce::jmin(grainRelease, grainDurationMs * 0.3f));
                grain.envelope->setSustainTime(grainDurationMs * 0.4f);
                
                DBG("Triggered grain [" << startPos << "-" << endPos << 
                    "] duration: " << grainDurationMs << "ms");
                return;
            }
        }
    }
    DBG("No inactive grains available for triggering");
}

} // namespace DSP