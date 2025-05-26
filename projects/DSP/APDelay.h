#pragma once

#include "DelayLine.h"
#include "Ramp.h"
#include "ParametricEqualizer.h"

namespace DSP
{

class Delay
{
public:

    Delay(float maxTimeMs, unsigned int numChannels);
    ~Delay();

    // No default ctor
    Delay() = delete;

    // No copy semantics
    Delay(const Delay&) = delete;
    const Delay& operator=(const Delay&) = delete;

    // No move semantics
    Delay(Delay&&) = delete;
    const Delay& operator=(Delay&&) = delete;

    // Update sample rate, reallocates and clear internal buffers
    void prepare(double sampleRate, float maxTimeMs, unsigned int numChannels);

    // Clear contents of internal buffer
    void clear();

    // Process audio
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // Set delay  in ms
    void setDelay(float newDelayMs);

    // Set feedback gain
    void setFeedback(float newFeedback);

    // Set mix gain
    void setMix(float newMix);

    // Set coloration
    void setColour(float newColour);
    
    
private:
    double sampleRate { 48000.0 };

    DSP::ParametricEqualizer filter;
    DSP::Ramp<float> delayRamp;
    DSP::DelayLine delayLine;

    std::vector<float> previousDelayOut;
    
    float delayMs { 0.f };
    float feedbackGain { 0.f };
    float toneFrequency { 2000.f};
    float mix { 0.f };
    float colour { 0.f };

};

}