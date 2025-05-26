#pragma once

namespace DSP
{

class RingMod
{
public:
    enum ModType : unsigned int
    {
        Sin = 0,
        Tri,
        Sqr
    };

    RingMod();
    ~RingMod() {}

    // No copy and move
    RingMod(const RingMod&) = delete;
    RingMod(RingMod&&) = delete;
    const RingMod& operator=(const RingMod&) = delete;
    const RingMod& operator=(RingMod&&) = delete;

    void prepare(double sampleRate);

    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // Set the modulation rate in Hz
    void setModRate(float modRateHz);

    // Set modulation type
    void setModType(ModType type);

private:
    double sampleRate { 48000.0 };

    // modulation rate variable
    float modRate { 0.f };

    // modulation type variable
    ModType modType { Sin };

    // Phase state of the oscillators
    // goes from "0.f" to "2.f * M_PI"
    float phaseState[2] { 0.f, 0.f };
    float phaseInc { 0.f };
};

}