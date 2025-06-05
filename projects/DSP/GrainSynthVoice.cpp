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
    
    if (granSynth && sampleBuffer)
    {
        granSynth->setBuffer(sampleBuffer);
        granSynth->setGrainEnv(grainAttack, grainSustain, grainRelease);
        granSynth->setGrainAmp(grainAmp * level);
        
        int startSample = static_cast<int>(filePositionInSamples * sampleBuffer->getNumSamples());
        int endSample = startSample + static_cast<int>(grainSize);
        
        endSample = juce::jmin(endSample, sampleBuffer->getNumSamples());
        
        granSynth->synthesize(0.5f, grainSize * 0.5f, grainSize * 1.5f);
    }
}

void GrainSynthVoice::stopNote(float, bool allowTailOff)
{
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
    
    // Process grains through granSynth
    granSynth->processBlock(grainBuffer);
    
    // Apply to output with pitch and level
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