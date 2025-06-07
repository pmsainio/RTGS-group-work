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
        DBG("[Grain] Attack" << grainAttack << "Release" << grainRelease << "Sustain" << grainSustain);
        granSynth->setGrainEnv(grainAttack, grainSustain, grainRelease);
        granSynth->setGrainAmp(grainAmp);
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
    grainSize = value; // value in milliseconds
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
        if (noteHeld && timeUntilNextGrain <= 0.0f)
        {
            float minMs = grainSize * 0.30f;
            float randMs = minMs + static_cast<float>(rand()) / RAND_MAX * grainSize;
            int actualGrainSizeSamples = grainSize + static_cast<int>(randMs * sampleRate * 0.001f);

            int maxStart = sampleBuffer->getNumSamples() - actualGrainSizeSamples;
            int startPos = std::clamp(static_cast<int>(filePositionInSamples), 0, maxStart);
            DBG("[Grain] FilePos" << filePositionInSamples);
            int endPos = startPos + actualGrainSizeSamples;

            DBG("[Grain] Start Pos: " << startPos);
            DBG("[Grain] End Pos: " << endPos);
            DBG("[Grain] Size (ms): " << randMs);

            if (granSynth->trigger(startPos, endPos, pitchRatio))
                timeUntilNextGrain += grainIntervalSamples;
        }

        timeUntilNextGrain -= 1.0f;
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
    DBG("[Grain] Density" << density);
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

void GrainSynthVoice::setFilePos(float value)
{
        if (sampleBuffer && sampleBuffer->getNumSamples() > 0)
        {
            int length = sampleBuffer->getNumSamples();
            filePositionInSamples = static_cast<int>(std::clamp(value, 0.0f, 1.0f) * length);
        }
}

void GrainSynthVoice::pitchWheelMoved(int) {}
void GrainSynthVoice::controllerMoved(int, int) {}