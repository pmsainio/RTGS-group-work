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
        DBG("Starting note - buffer has " << sampleBuffer->getNumSamples() << " samples");
        
        granSynth->setBuffer(sampleBuffer);
        granSynth->setGrainEnv(grainAttack, grainSustain, grainRelease);
        granSynth->setGrainAmp(0.1 * grainAmp * level);
        
        float density = 10.0f;
        
        DBG("Triggering grains with size " << minSize << "-" << maxSize << " samples");
        granSynth->synthesize(density, minSize, maxSize);
    }
}

void GrainSynthVoice::setMinSize(float newMinSize)
{
    minSize = newMinSize;
}

void GrainSynthVoice::setMaxSize(float newMaxSize)
{
    maxSize = newMaxSize;
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
    
    granSynth->processBlock(grainBuffer);

    for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
    {
        outputBuffer.addFrom(ch, startSample, grainBuffer, ch, 0, numSamples, level);
    }

    DBG("Render - Playing: " << (int)isPlaying 
    << ", GranSynth: " << (granSynth ? "valid" : "null") 

    << ", Buffer: " << (sampleBuffer ? "valid" : "null"));
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