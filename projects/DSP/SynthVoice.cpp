#include "SynthVoice.h"

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

void SynthVoice::setAttackTime(float newAttackTimeMs)
{
    ramp.setRampTime(newAttackTimeMs * 0.001f);
    env.setAttackTime(newAttackTimeMs);
}

void SynthVoice::setDecayTime(float decayTimeMs) { env.setDecayTime(decayTimeMs); }
void SynthVoice::setSustainLevel(float sustainLevel) { env.setSustainLevel(sustainLevel); }

void SynthVoice::setReleaseTime(float newReleaseTimeMs)
{
    ramp.setRampTime(newReleaseTimeMs * 0.001f);
    env.setReleaseTime(newReleaseTimeMs);
}

// just check if this is "ours" synth sound
bool SynthVoice::canPlaySound(SynthesiserSound* ptr)
{
    if (dynamic_cast<SynthSound*>(ptr))
        return true;
    return false;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound*, int)
{
    ramp.setTarget(velocity);
    osc.setFrequency(convertMidiNoteToFreq(midiNoteNumber));
    env.start();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    ramp.setTarget(0.f);
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
        ramp.prepare(sampleRate);
        env.prepare(sampleRate);
    }

    auto* outputPtr = outputBuffer.getWritePointer(0, startSample);

    // process the oscillator
    osc.process(outputPtr, static_cast<unsigned int>(numSamples));

    // apply simple ramp as VCA
    ramp.applyGain(&outputPtr, 1u, static_cast<unsigned int>(numSamples));

    // apply envelope
    std::vector<float> envelopeBuffer(static_cast<size_t>(numSamples));
    env.process(envelopeBuffer.data(), static_cast<unsigned int>(numSamples));

    // Apply envelope modulation ramp for final amplitude
    for (int sample = 0; sample < numSamples; ++sample)
    {
        outputPtr[sample] *= envelopeBuffer[sample];
    }

    // if the output is stereo, just copy contents from left to right channel
    if (outputBuffer.getNumChannels() > 1)
        outputBuffer.copyFrom(1, startSample, outputBuffer, 0, startSample, numSamples);
}

}
