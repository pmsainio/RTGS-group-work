#include "GranSynth.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include <cstdlib>

namespace DSP {

GranSynth::GranSynth(double sampleRate) : sampleRate(sampleRate)
{
    grains.resize(64);
    for (auto& grain : grains)
        grain.envelope = std::make_unique<GrainEnvelope>();
}

GranSynth::~GranSynth() = default;

float GranSynth::frandom()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void GranSynth::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    for (auto& grain : grains)
        grain.envelope->prepare(sampleRate, 0);
}

void GranSynth::setBuffer(juce::AudioBuffer<float>* buffer)
{
    sampleBuffer = buffer;
    fileLength = buffer ? buffer->getNumSamples() : 0;
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

        float* envelopeOut = new float[numSamples];
        grain.envelope->process(envelopeOut, numSamples);

        for (int i = 0; i < numSamples; ++i)
        {
            if (grain.envelope->isOff())
            {
                grain.active = false;
                break;
            }

            float sample = 0.0f;

            if (grain.currPos < grain.endPos && grain.currPos < sampleBuffer->getNumSamples())
            {
                int idx = static_cast<int>(grain.currPosFloat);
                float frac = grain.currPosFloat - static_cast<float>(idx);

                float s1 = sampleBuffer->getSample(0, idx);
                float s2 = sampleBuffer->getSample(0, std::min(idx + 1, sampleBuffer->getNumSamples() - 1));
                sample = s1 + frac * (s2 - s1);

                grain.currPosFloat += grain.pitchRatio;
                grain.currPos = static_cast<int>(grain.currPosFloat);

                if (grain.currPos >= grain.endPos)
                    grain.envelope->end();  // ensure we enter RELEASE
            }

            float env = envelopeOut[i];
            float out = sample * env * grain.amplitude;

            outputBuffer.addSample(0, i, out);
            if (numChannels > 1)
                outputBuffer.addSample(1, i, out);
        }

        delete[] envelopeOut;
    }
}

void GranSynth::setGrainEnv(float attack, float sustain, float release)
{
    grainAttack = attack;
    grainSustain = sustain;
    grainRelease = release;
}

void GranSynth::synthesize(float density, float minSize, float maxSize)
{
    if (!sampleBuffer || fileLength <= 0)
        return;

    const int numGrains = static_cast<int>(density);
    
    for (int i = 0; i < numGrains; ++i)
    {
        float grainSize = minSize + frandom() * (maxSize - minSize);
        int sizeSamples = static_cast<int>(grainSize);

        sizeSamples = std::clamp(sizeSamples, 16, fileLength - 1);

        int startPos = static_cast<int>(frandom() * (fileLength - sizeSamples));
        int endPos = startPos + sizeSamples;

        trigger(startPos, endPos, 1.0f); 
    }
}


bool GranSynth::trigger(int startPos, int endPos, float pitchRatio)
{
    for (auto& grain : grains)
    {
        if (!grain.active)
        {
            grain.startPos = startPos;
            grain.endPos = endPos;
            grain.currPos = startPos;
            grain.currPosFloat = static_cast<float>(startPos);
            grain.pitchRatio = pitchRatio;
            grain.amplitude = grainAmplitude;
            grain.active = true;

            float durationMs = 1000.0f * (endPos - startPos) / sampleRate;

            float attackMs  = grainAttack;
            float releaseMs = grainRelease;
            float sustainMs;

            if (attackMs + releaseMs >= durationMs)
            {
                float segment = durationMs / 3.0f;
                attackMs  = segment;
                sustainMs = segment;
                releaseMs = segment;
                DBG("[Grain] Fallback to thirds: duration too short for custom env");
            }
            else
            {
                sustainMs = durationMs - (attackMs + releaseMs);
            }

            if (grain.envelope)
            {
                grain.envelope->setGrainAmplitude(grainSustain);
                grain.envelope->setAttackTime(attackMs);
                grain.envelope->setSustainTime(sustainMs);
                grain.envelope->setReleaseTime(releaseMs);
                grain.envelope->prepare(sampleRate, 0);
                grain.envelope->start();
            }

            DBG("[Grain] Start: " << startPos << ", End: " << endPos << ", Duration(ms): " << durationMs);
            DBG("[Grain] Attack(ms): " << attackMs);
            DBG("[Grain] Sustain(ms): " << sustainMs);
            DBG("[Grain] Release(ms): " << releaseMs);
            DBG("[Grain] Amplitude: " << grainAmplitude);
            DBG("[Grain] Grain Size:" << durationMs);

            return true;
        }
    }

    return false;
}


int GranSynth::getActiveGrainCount() const
{
    return std::count_if(grains.begin(), grains.end(), [](const auto& g) { return g.active; });
}

} // namespace DSP
