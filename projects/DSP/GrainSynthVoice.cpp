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
        DBG("Starting note - buffer has " << sampleBuffer->getNumSamples() << " samples");

        grainIntervalSamples = sampleRate / density;
        timeUntilNextGrain = 0.0;
        
        granSynth->setBuffer(sampleBuffer);
        granSynth->setGrainEnv(grainAttack, grainSustain, grainRelease);
        granSynth->setGrainAmp(0.1 * grainAmp * level);
        
        DBG("Triggering grains with size " << minSize << "-" << maxSize << " samples");
    }
}

void GrainSynthVoice::setLevel(float newLevel)
{
    level = newLevel;
}

void GrainSynthVoice::setMinSize(float newMinSize)
{
    minSize = newMinSize;
}

void GrainSynthVoice::setMaxSize(float newMaxSize)
{
    maxSize = newMaxSize;
}

void GrainSynthVoice::setDensity(float newDensity)
{
    density = newDensity;
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

void GrainSynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                    int startSample, int numSamples)
{
    if (!isPlaying || !granSynth || !sampleBuffer) return;

    juce::AudioBuffer<float> grainBuffer(outputBuffer.getNumChannels(), numSamples);
    grainBuffer.clear();

    for (int i = 0; i < numSamples; ++i)
    {
        if (noteHeld && timeUntilNextGrain <= 0.0)
        {
            float grainSize = (minSize + (rand() / (float)RAND_MAX) * (maxSize - minSize));
            int startPos = static_cast<int>((sampleBuffer->getNumSamples() - grainSize) * (rand() / (float)RAND_MAX));
            int endPos = startPos + static_cast<int>(grainSize);

            bool success = granSynth->trigger(startPos, endPos, pitchRatio);

            if (success)
                timeUntilNextGrain += grainIntervalSamples;
        }

        timeUntilNextGrain -= 1.0;
    }

    // Then process grains as usual
    granSynth->processBlock(grainBuffer);

    // Add to output
    for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
    {
        outputBuffer.addFrom(ch, startSample, grainBuffer, ch, 0, numSamples, level);
    }
}

void GrainSynthVoice::setSampleBuffer(juce::AudioBuffer<float>* buffer)
{
    sampleBuffer = buffer;
}

void GrainSynthVoice::setGranSynth(DSP::GranSynth* newGranSynth)
{
    granSynth = newGranSynth;
    if (granSynth)
    {
        granSynth->prepare(sampleRate);
    }
}

void GrainSynthVoice::setGrainParameters(float attackMs, float sustainMs, float releaseMs, float amplitude)
{
    grainAttack = attackMs;
    grainSustain = sustainMs;
    grainRelease = releaseMs;
    grainAmp = amplitude;
}

void GrainSynthVoice::pitchWheelMoved(int) {}
void GrainSynthVoice::controllerMoved(int, int) {}