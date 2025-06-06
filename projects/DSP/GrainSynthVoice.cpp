#include "GrainSynthVoice.h"

GrainSynthVoice::GrainSynthVoice() {}

bool GrainSynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void GrainSynthVoice::startNote(int midiNoteNumber, float velocity,
                                 juce::SynthesiserSound*, int)
{
    pitchRatio = std::pow(2.0, (midiNoteNumber - 60) / 12.0);
    level = velocity;
    isPlaying = true;
    noteHeld = true;

    if (granSynth && sampleBuffer)
    {
        grainIntervalSamples = sampleRate / density;
        timeUntilNextGrain = 0.0;

        granSynth->setBuffer(sampleBuffer);
    }
}

void GrainSynthVoice::stopNote(float, bool allowTailOff)
{
    noteHeld = false;
    if (!allowTailOff)
    {
        isPlaying = false;
        clearCurrentNote();
    }
}

void GrainSynthVoice::setMaxSize(float value)
{
    grainSize = static_cast<int>(value * sampleRate * 0.001f); // convert from ms to samples
}

void GrainSynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                      int startSample, int numSamples)
{
    if (!isPlaying || !granSynth || !sampleBuffer) return;

    const int bufferLength = sampleBuffer->getNumSamples();
    if (bufferLength <= 0) return;

    juce::AudioBuffer<float> grainBuffer(outputBuffer.getNumChannels(), numSamples);
    grainBuffer.clear();

    for (int i = 0; i < numSamples; ++i)
    {
        if (noteHeld && timeUntilNextGrain <= 0.0)
        {
            const int minGrainSizeSamples = static_cast<int>(sampleRate * 0.030f);
            int maxGrainSizeSamples = static_cast<int>(std::max(grainSize, 30.0f) * sampleRate * 0.001f);
            int actualGrainSize = minGrainSizeSamples;
            if (maxGrainSizeSamples > minGrainSizeSamples)
            {
                float randFactor = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                actualGrainSize = minGrainSizeSamples + static_cast<int>(
                    randFactor * (maxGrainSizeSamples - minGrainSizeSamples));
            }

            int maxStart = sampleBuffer->getNumSamples() - actualGrainSize;
            int startPos = std::min(filePositionInSamples, maxStart);
            int endPos = startPos + actualGrainSize;

            if (granSynth->trigger(startPos, endPos, pitchRatio))
                timeUntilNextGrain += grainIntervalSamples;
        }

        timeUntilNextGrain -= 1.0;
    }

    granSynth->processBlock(grainBuffer);

    for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
    {
        outputBuffer.addFrom(ch, startSample, grainBuffer, ch, 0, numSamples, level);
    }

    if (!noteHeld && granSynth->getActiveGrainCount() == 0)
    {
        isPlaying = false;
        clearCurrentNote();
    }
}

void GrainSynthVoice::setDensity(float newdensity)
{
    density = newdensity;
}

void GrainSynthVoice::setSampleBuffer(juce::AudioBuffer<float>* buffer)
{
    sampleBuffer = buffer;
}

void GrainSynthVoice::setGranSynth(DSP::GranSynth* newGranSynth)
{
    granSynth = newGranSynth;
    if (granSynth)
        granSynth->prepare(sampleRate);
}

void GrainSynthVoice::pitchWheelMoved(int) {}
void GrainSynthVoice::controllerMoved(int, int) {}