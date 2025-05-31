#include "Granulator.h"
#include "juce_audio_basics/juce_audio_basics.h"

float DSP::Granulator::cubicInterp(float x, float y0, float y1, float y2, float y3)
{
    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);
    float c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
    float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
        
    return ((c3 * x + c2) * x + c1) * x + c0;
}

void DSP::Granulator::prepare(float newSampleRate, unsigned int numChannels, int newBlockSize)
{
    sampleRate = newSampleRate;
}

