#include "Grain.h"

Grain::Grain()
{

}

void Grain::prepare(double sampleRate, unsigned int rndStartSamples) {
    envelope.prepare(sampleRate, rndStartSamples);
}

void Grain::setGrainASR(float attackTimeMs, float sustainTimeMs, float releaseTimeMs) {
    envelope.setAttackTime(attackTimeMs);
    envelope.setSustainTime(sustainTimeMs);
    envelope.setReleaseTime(releaseTimeMs);

}

void Grain::process()
{
    envelope.process();
}
