#include "APDelay.h"

#include <cmath>

namespace DSP
{

Delay::Delay(float maxTimeMs, unsigned int numChannels) :
    delayLine(1, numChannels),
    delayRamp(0.05f),
    filter(1)
{
}

Delay::~Delay()
{
}

void Delay::prepare(double newSampleRate, float maxTimeMs, unsigned int numChannels)
{
    sampleRate = newSampleRate;

    delayLine.prepare(static_cast<unsigned int>(std::round(maxTimeMs * static_cast<float>(0.001 * sampleRate))), numChannels);
    delayRamp.prepare(sampleRate, true, delayMs * static_cast<float>(0.001 * sampleRate));
    previousDelayOut.resize(numChannels, 0.0f);

    // Prepare lowpass filter for colouration
    filter.prepare(sampleRate, numChannels);
    filter.setBandType(0, DSP::ParametricEqualizer::LowPass);
    filter.setBandFrequency(0, toneFrequency);
    filter.setBandGain(0, -3.0f);
    filter.setBandResonance(0, 0.7071f);

    clear();
}

void Delay::clear()
{
    delayLine.clear();
}

void Delay::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{
    for (unsigned int n = 0; n < numSamples; ++n)
    {   
        float curDelaySamples = 1.0f; 
        delayRamp.applyGain(&curDelaySamples, 1); // get next sample from ramp
        unsigned int ds = static_cast<unsigned int>(std::round(curDelaySamples)); // convert to samples
        delayLine.setDelaySamples(ds); // set delay using the ramp
        
        // Delay in/out
        float x[2];
        float y[2];

        for (unsigned int ch = 0; ch < numChannels; ++ch)
        {
            x[ch] = input[ch][n] + feedbackGain * previousDelayOut[ch]; // adding feedback to input
            x[ch] = std::tanh(x[ch] * 2 * colour) / colour; // apply colouration

        }
        float filterOut[2];
        filter.process(filterOut, x, numChannels); // apply lowpass filter

        // Process delay
        delayLine.process(y, filterOut, numChannels);

        // Write to output buffers
        for (unsigned int ch = 0; ch < numChannels; ++ch)
        {
            output[ch][n] = input[ch][n] * (1.0f - mix) + y[ch] * mix; // dry/wet mixing 
            previousDelayOut[ch] = y[ch]; // updating previous delay output
        }
    }
}

void Delay::setDelay(float newDelayMs)
{
    delayRamp.setTarget(newDelayMs * static_cast<float>(0.001 * sampleRate));
    delayMs = newDelayMs * static_cast<float>(0.001 * sampleRate);
    delayLine.setDelaySamples(delayMs);
}

void Delay::setFeedback(float newFeedback)
{
    feedbackGain = newFeedback;
}

void Delay::setMix(float newMix)
{
    mix = newMix;
}

void Delay::setColour(float newColour)
{
    colour = newColour;
}
}