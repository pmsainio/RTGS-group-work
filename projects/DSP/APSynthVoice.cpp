#include "APSynthVoice.h"

namespace DSP
{

float convertMidiNoteToFreq(int MidiNote)
{
    return 440.f * std::pow(2.f, static_cast<float>(MidiNote - 69) / 12.f);
}

SynthVoice::SynthVoice()
{
}

SynthVoice::~SynthVoice()
{
}


void SynthVoice::setWaveType(Oscillator::OscType type)
{
    osc.setType(type);
}

void SynthVoice::setAttackTime(float newAttackTimeMs) { env.setAttackTime(newAttackTimeMs);}
void SynthVoice::setDecayTime(float decayTimeMs) { env.setDecayTime(decayTimeMs); }
void SynthVoice::setSustainLevel(float sustainLevel) { env.setSustainLevel(sustainLevel); }
void SynthVoice::setReleaseTime(float newReleaseTimeMs){ env.setReleaseTime(newReleaseTimeMs) ;}
void SynthVoice::setCutoffFreq(float newCutoffFreq){ cutoffRamp.setTarget(newCutoffFreq);}
void SynthVoice::setResonance(float newResonance){ resoRamp.setTarget(newResonance);}

// just check if this is "ours" synth sound
bool SynthVoice::canPlaySound(SynthesiserSound* ptr)
{
    if (dynamic_cast<SynthSound*>(ptr))
        return true;
    return false;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound*, int)
{
    osc.setFrequency(convertMidiNoteToFreq(midiNoteNumber));
    env.start();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    env.end();
}

void SynthVoice::pitchWheelMoved(int)
{
}

void SynthVoice::controllerMoved(int, int)
{
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    // there's no "prepare" call to synth voice, so we need to manually check
    // every buffer call if the sample rate has changed
    double newSampleRate { getSampleRate() };
    if (newSampleRate != sampleRate)
    {
        sampleRate = newSampleRate;
        osc.prepare(sampleRate);
        env.prepare(sampleRate);
        svfLeft.prepare(sampleRate);
        svfRight.prepare(sampleRate);
        freqInBuffer.setSize(1, numSamples);
        resoInBuffer.setSize(1, numSamples);
        lpfOutBuffer.setSize(2, numSamples);
    }

    auto* outputPtr = outputBuffer.getWritePointer(0, startSample);
    // process the oscillator
    osc.process(outputPtr, static_cast<unsigned int>(numSamples));

    // process the filter
    freqInBuffer.clear();
    resoInBuffer.clear();
    lpfOutBuffer.clear();

    cutoffRamp.applySum(freqInBuffer.getWritePointer(0), numSamples);
    resoRamp.applySum(resoInBuffer.getWritePointer(0), numSamples);
    svfLeft.process(lpfOutBuffer.getWritePointer(0),
                    nullptr,
                    nullptr,
                    outputPtr,
                    freqInBuffer.getReadPointer(0),
                    resoInBuffer.getReadPointer(0),
                    numSamples);

    // if stereo, also process right channel
    if (outputBuffer.getNumChannels() > 1)
    {
        svfRight.process(lpfOutBuffer.getWritePointer(1),
                         nullptr,
                         nullptr,
                         outputPtr,
                         freqInBuffer.getReadPointer(0),
                         resoInBuffer.getReadPointer(0),
                         numSamples);
    }
    // apply envelope
    auto* lpfOutputPtr = lpfOutBuffer.getWritePointer(0, startSample);
    std::vector<float> envelopeBuffer(static_cast<size_t>(numSamples));
    env.process(envelopeBuffer.data(), static_cast<unsigned int>(numSamples));
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        lpfOutputPtr[sample] *= envelopeBuffer[sample];
    }

    // if the output is stereo, just copy contents from left to right channel
    if (outputBuffer.getNumChannels() > 1)
        outputBuffer.copyFrom(1, startSample, outputBuffer, 0, startSample, numSamples);
}

}
