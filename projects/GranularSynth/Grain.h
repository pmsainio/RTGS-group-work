#ifndef GRAIN_H
#define GRAIN_H

#include "../DSP/GrainEnvelope.h"

class Grain {
private:
    GrainEnvelope envelope;

public:
    Grain();
    void prepare(double sampleRate, unsigned int rndStartSamples);
    void setGrainASR(float attackTimeMs, float sustainTimeMs, float releaseTimeMs);
    void process();
};

#endif // GRAIN_H
