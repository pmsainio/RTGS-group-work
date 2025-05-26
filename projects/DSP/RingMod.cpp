#include "RingMod.h"

#include <cmath>
#include <algorithm>

namespace DSP
{

RingMod::RingMod()
{
}

void RingMod::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;

    // reset phase state
    phaseState[0] = 0.f;
    phaseState[1] = static_cast<float>(M_PI / 2.0); // quadature osc between L and R channels

    // update phase increment for new sample rate
    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * modRate;
}

void RingMod::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{
    numChannels = std::min(numChannels, 2u);
    for (unsigned int n = 0; n < numSamples; ++n)
    {
        // Process LFO acording to mod type
        float lfo[2] { 0.f, 0.f };
        switch (modType)
        {
        case Sin:
            lfo[0] = std::sin(phaseState[0]);
            lfo[1] = std::sin(phaseState[1]);
            break;

        case Tri:
            lfo[0] = 2.f * std::fabs((phaseState[0] - static_cast<float>(M_PI)) * static_cast<float>(1.0 / M_PI)) - 1.f;
            lfo[1] = 2.f * std::fabs((phaseState[1] - static_cast<float>(M_PI)) * static_cast<float>(1.0 / M_PI)) - 1.f;
            break;

        case Sqr:
            lfo[0] = std::copysign(1.f, phaseState[0] - static_cast<float>(M_PI));
            lfo[1] = std::copysign(1.f, phaseState[1] - static_cast<float>(M_PI));
            break;
        }

        // Increment and wrap phase states
        phaseState[0] = std::fmod(phaseState[0] + phaseInc, static_cast<float>(2 * M_PI));
        phaseState[1] = std::fmod(phaseState[1] + phaseInc, static_cast<float>(2 * M_PI));

        // Do amplitude modulation
        for (unsigned int ch = 0; ch < numChannels; ++ch)
            output[ch][n] = lfo[ch] * input[ch][n];
    }
}

void RingMod::setModRate(float newModRate)
{
    modRate = std::fmax(newModRate, 0.f);
    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * modRate;
}

void RingMod::setModType(ModType type)
{
    modType = type > Sqr ? Sqr : type;
}


}