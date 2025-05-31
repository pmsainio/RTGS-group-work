#pragma once

namespace DSP
{

class GrainEnvelope
{
public:
    GrainEnvelope();
    ~GrainEnvelope();

    GrainEnvelope(const GrainEnvelope&) = delete;
    GrainEnvelope(GrainEnvelope&&) = delete;
    const GrainEnvelope& operator=(const GrainEnvelope&) = delete;
    const GrainEnvelope& operator=(GrainEnvelope&&) = delete;

    void prepare(double newSampleRate);
    void process(float* output, unsigned int numSamples);
    void start();
    void end();
    bool isOff() const { return state == OFF; }
    void setAttackTime(float attackTimeMs);
    void setGrainAmplitude(float sustainLevelLinear);
    void setSustainTime(float sustainTimeMs);
    void setReleaseTime(float releaseTimeMs);

private:
    double sampleRate { 48000.0 };
    float attackTimeMs { 10.f };
    float sustainTimeMs { 100.f };
    float releaseTimeMs { 50.f };
    float grainAmplitude { 1.f };
    unsigned int attackTimeSamples { 0 };
    unsigned int sustainTimeSamples { 0 };
    unsigned int releaseTimeSamples { 0 };
    unsigned int attackSamplesCounter { 0 };
    unsigned int sustainSamplesCounter { 0 };
    unsigned int releaseSamplesCounter { 0 };
    float currentEnvelope { 0.f };
    enum EnvelopeState : unsigned int
    {
        OFF = 0,
        ATTACK,
        SUSTAIN,
        RELEASE
    };
    EnvelopeState state { OFF };
    void doDigital(float* output, unsigned int numSamples);
};

}