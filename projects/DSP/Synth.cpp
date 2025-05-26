#include "Synth.h"

namespace DSP
{

float convertMidiNoteToFreq(int MidiNote)
{
    return 440.f * std::pow(2.f, static_cast<float>(MidiNote - 69) / 12.f);
}

SynthVoice::SynthVoice()
{
    sawOsc.setType(Oscillator::SawAA);
    triOsc.setType(Oscillator::TriAA);
    sinOsc.setType(Oscillator::Sin);

    vcaEnvGen.setAnalogStyle(false);
    vcfEnvGen.setAnalogStyle(false);
}

SynthVoice::~SynthVoice()
{
}

void SynthVoice::setOscSawVol(float dB, bool skipRamp)
{
    sawOscVolRamp.setTarget(std::pow(10.f, 0.05f * dB), skipRamp);
}

void SynthVoice::setOscTriVol(float dB, bool skipRamp)
{
    triOscVolRamp.setTarget(std::pow(10.f, 0.05f * dB), skipRamp);
}

void SynthVoice::setOscSinVol(float dB, bool skipRamp)
{
    sinOscVolRamp.setTarget(std::pow(10.f, 0.05f * dB), skipRamp);
}

void SynthVoice::setOscVol(float dB, bool skipRamp)
{
    oscVolRamp.setTarget(std::pow(10.f, 0.05f * dB), skipRamp);
}

void SynthVoice::setAttTimeVCA(float ms)
{
    vcaEnvGen.setAttackTime(ms);
}

void SynthVoice::setDecayTimeVCA(float ms)
{
    vcaEnvGen.setDecayTime(ms);
}

void SynthVoice::setSustainVCA(float norm)
{
    vcaEnvGen.setSustainLevel(std::clamp(norm, 0.f, 1.f));
}

void SynthVoice::setRelTimeVCA(float ms)
{
    vcaEnvGen.setReleaseTime(ms);
}

void SynthVoice::setAttTimeVCF(float ms)
{
    vcfEnvGen.setAttackTime(ms);
}

void SynthVoice::setDecayTimeVCF(float ms)
{
    vcfEnvGen.setDecayTime(ms);
}

void SynthVoice::setSustainVCF(float norm)
{
    vcfEnvGen.setSustainLevel(std::clamp(norm, 0.f, 1.f));
}

void SynthVoice::setRelTimeVCF(float ms)
{
    vcfEnvGen.setReleaseTime(ms);
}

void SynthVoice::setLFOFreqVCF(float Hz)
{
    lfoFreq = std::fmax(Hz, 0.f);
    lfoPhaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * lfoFreq;
}

void SynthVoice::setLFOTypeVCF(LFOType type)
{
    lfoType = type;
}

void SynthVoice::setEnvAmountVCF(float bipolar, bool skipRamp)
{
    vcfEnvAmountRamp.setTarget(std::clamp(bipolar, -1.f, 1.f), skipRamp);
}

void SynthVoice::setLFOAmountVCF(float bipolar, bool skipRamp)
{
    vcfLFOAmountRamp.setTarget(std::clamp(bipolar, -1.f, 1.f), skipRamp);
}

void SynthVoice::setFilterCutoff(float Hz, bool skipRamp)
{
    vcfFreqRamp.setTarget(std::clamp(Hz, MinFreqHz, MaxFreqHz), skipRamp);
}

void SynthVoice::setFilterReso(float Q, bool skipRamp)
{
    vcfResoRamp.setTarget(std::clamp(Q, MinReso, MaxReso), skipRamp);
}

void SynthVoice::setFilterType(FilterType type, bool skipRamp)
{
    vcfLPFRamp.setTarget(type == LPF ? 1.f : 0.f, skipRamp);
    vcfBPFRamp.setTarget(type == BPF ? 1.f : 0.f, skipRamp);
    vcfHPFRamp.setTarget(type == HPF ? 1.f : 0.f, skipRamp);
}

void SynthVoice::setOutputVol(float dB, bool skipRamp)
{
    outputVolRamp.setTarget(std::pow(10.f, 0.05f * dB), skipRamp);
}


bool SynthVoice::canPlaySound(juce::SynthesiserSound* ptr)
{
    return true;
}

void SynthVoice::startNote(int midiNoteNumber, float newVelocity, juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    sinOsc.setFrequency(convertMidiNoteToFreq(midiNoteNumber));
    triOsc.setFrequency(convertMidiNoteToFreq(midiNoteNumber));
    sawOsc.setFrequency(convertMidiNoteToFreq(midiNoteNumber));
    vcaEnvGen.start();
    vcfEnvGen.start();

    velocity = newVelocity;
    voiceStarted = true;
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    vcaEnvGen.end();
    vcfEnvGen.end();

    if (!allowTailOff)
        clearCurrentNote();
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    const auto newSampleRate { getSampleRate() };
    if (sampleRate != newSampleRate)
    {
        sampleRate = newSampleRate;

        sinOsc.prepare(sampleRate);
        triOsc.prepare(sampleRate);
        sawOsc.prepare(sampleRate);
        vcaEnvGen.prepare(sampleRate);
        vcfEnvGen.prepare(sampleRate);
        filter.prepare(sampleRate);
        sinOscVolRamp.prepare(sampleRate);
        triOscVolRamp.prepare(sampleRate);
        sawOscVolRamp.prepare(sampleRate);
        oscVolRamp.prepare(sampleRate);
        outputVolRamp.prepare(sampleRate);
        vcfEnvAmountRamp.prepare(sampleRate);
        vcfLFOAmountRamp.prepare(sampleRate);
        vcfFreqRamp.prepare(sampleRate);
        vcfResoRamp.prepare(sampleRate);
        vcfLPFRamp.prepare(sampleRate);
        vcfBPFRamp.prepare(sampleRate);
        vcfHPFRamp.prepare(sampleRate);

        lfoPhaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * std::fmax(lfoFreq, 0.f);
    }

    for (int i = 0; i < numSamples; ++i)
    {
        const auto sin { sinOsc.process() };
        const auto tri { triOsc.process() };
        const auto saw { sawOsc.process() };

        float vcaEnv { 0.f };
        vcaEnvGen.process(&vcaEnv, 1);

        float vcfEnv { 0.f };
        vcfEnvGen.process(&vcfEnv, 1);

        const auto sinVol { sinOscVolRamp.getNext() };
        const auto triVol { triOscVolRamp.getNext() };
        const auto sawVol { sawOscVolRamp.getNext() };
        const auto oscVol { oscVolRamp.getNext() };

        const auto vcfEnvAmout { vcfEnvAmountRamp.getNext() };
        const auto vcfLFOAmount { vcfLFOAmountRamp.getNext() };

        const auto vcfFreq { vcfFreqRamp.getNext() };
        const auto vcfReso { vcfResoRamp.getNext() };
        const auto vcfLPF { vcfLPFRamp.getNext() };
        const auto vcfBPF { vcfBPFRamp.getNext() };
        const auto vcfHPF { vcfHPFRamp.getNext() };

        const auto outputVol { outputVolRamp.getNext() };

        // Process LFO acording to mod type
        float lfo { 0.f };
        switch (lfoType)
        {
        case TRI:
            lfo = std::fabs((lfoPhaseState - static_cast<float>(M_PI)) / static_cast<float>(M_PI));
            break;

        case SIN:
            lfo = 0.5f + 0.5f * std::sin(lfoPhaseState);
            break;
        }
        lfoPhaseState = std::fmod(lfoPhaseState + lfoPhaseInc, static_cast<float>(2 * M_PI));

        const auto oscOut { (sin * sinVol + tri * triVol + saw * sawVol) * oscVol * vcaEnv * velocity };
        const auto freqMod { std::clamp(vcfEnv * vcfEnvAmout + vcfLFOAmount * lfo, -1.f, 1.f) };
        const auto freq { std::clamp(FreqModRange * (std::pow(2.f, freqMod) - 1.f) + vcfFreq, MinFreqHz, MaxFreqHz) };

        float lpfOut { 0.f };
        float bpfOut { 0.f };
        float hpfOut { 0.f };
        filter.process(&lpfOut, &bpfOut, &hpfOut, &oscOut, &freq, &vcfReso, 1);

        const auto out { (vcfLPF * lpfOut + vcfBPF * bpfOut + vcfHPF * hpfOut) * outputVol };
        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
        {
            outputBuffer.addSample(ch, startSample + i, out);
        }

        if (voiceStarted && vcaEnvGen.isOff() && vcfEnvGen.isOff())
        {
            voiceStarted = false;
            clearCurrentNote();
        }
    }
}

}
