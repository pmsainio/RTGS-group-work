#ifndef GRAIN_H
#define GRAIN_H

#include "../DSP/GrainEnvelope.h"

class Grain {
private:
    DSP::GrainEnvelope envelope;

public:
    Grain();
    void prepare(double sampleRate, unsigned int rndStartSamples);
    void setGrainASR(float attackTimeMs, float sustainTimeMs, float releaseTimeMs);
    void process(float* output, unsigned int numSamples, unsigned int start, unsigned int end);
    unsigned int start { 0 };
    unsigned int end { 0 };
};

#endif // GRAIN_H
