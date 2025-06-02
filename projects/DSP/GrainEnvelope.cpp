#define _USE_MATH_DEFINES
#include "GrainEnvelope.h"
#include "juce_audio_basics/juce_audio_basics.h"

#include <cmath>

namespace DSP
{
    GrainEnvelope::GrainEnvelope()
    {
    }

    GrainEnvelope::~GrainEnvelope()
    {
    }

    void GrainEnvelope::prepare(double newSampleRate, unsigned int rndStartSamples)
    {
        sampleRate = newSampleRate;
        attackTimeSamples = std::rint(attackTimeMs*static_cast<float>(sampleRate*0.001));
        sustainTimeSamples = std::rint(sustainTimeMs*static_cast<float>(sampleRate*0.001));
        releaseTimeSamples = std::rint(releaseTimeMs*static_cast<float>(sampleRate*0.001));
        state = OFF;
        attackSamplesCounter = 0;
        sustainSamplesCounter = 0;
        releaseSamplesCounter = 0;
        if (rndStartSamples < attackTimeSamples)
        {
            state = ATTACK;
            attackSamplesCounter = rndStartSamples;
        }
        else if (rndStartSamples < attackTimeSamples + sustainTimeSamples)
        {
            state = SUSTAIN;
            sustainSamplesCounter = rndStartSamples;
        }
        else
        {
            state = RELEASE;
            releaseSamplesCounter = rndStartSamples;
        }
    }

    void GrainEnvelope::process(float* output, unsigned int numSamples)
    {
        doDigital(output, numSamples);
    }

    void GrainEnvelope::start()
    {
        state = ATTACK;
        attackSamplesCounter = 0;
        sustainSamplesCounter = 0;
        releaseSamplesCounter = 0;
    }

    void GrainEnvelope::end()
    {
        state = RELEASE;
        attackSamplesCounter = 0;
        sustainSamplesCounter = 0;
        releaseSamplesCounter = 0;
    }

    void GrainEnvelope::setAttackTime(float newAttackTimeMs)
    {
        attackTimeMs = std::fmax(newAttackTimeMs, 0.1f);
        attackTimeSamples = std::rint(attackTimeMs*static_cast<float>(sampleRate*0.001));
        if (state == ATTACK)
            attackSamplesCounter = std::min(attackTimeSamples-1, attackSamplesCounter);
    }

    void GrainEnvelope::setGrainAmplitude(float newGrainAmplitude)
    {
        grainAmplitude = std::clamp(newGrainAmplitude, 0.f, 1.f);
    }

    void GrainEnvelope::setSustainTime(float newSustainTimeMs)
    {
        sustainTimeMs = std::fmax(newSustainTimeMs, 0.1f);
        sustainTimeSamples = std::rint(sustainTimeMs*static_cast<float>(sampleRate*0.001));
        if (state == SUSTAIN)
            sustainSamplesCounter = std::min(sustainTimeSamples-1, sustainSamplesCounter);
    }

    void GrainEnvelope::setReleaseTime(float newReleaseTimeMs)
    {
        releaseTimeMs = std::fmax(newReleaseTimeMs, 0.1f);
        releaseTimeSamples = std::rint(releaseTimeMs*static_cast<float>(sampleRate*0.001));
        if (state == RELEASE)
            releaseSamplesCounter = std::min(releaseTimeSamples-1, releaseSamplesCounter);
    }

    void GrainEnvelope::doDigital(float* output, unsigned int numSamples)
    {
        for (unsigned int n=0; n<numSamples; ++n)
        {
            switch (state)
            {
            case OFF:
                {
                    currentEnvelope = 0.f;
                }
                break;

            case ATTACK:
                if (attackSamplesCounter < attackTimeSamples)
                {
                    currentEnvelope = (1.f + std::cos(M_PI + (M_PI * (attackSamplesCounter/attackTimeSamples)))) * (grainAmplitude/2.f);
                    currentEnvelope = std::fmin(currentEnvelope, 1.f);
                    ++attackSamplesCounter;
                }
                else
                {
                    attackSamplesCounter = 0;
                    state = SUSTAIN;
                }
                break;

            case SUSTAIN:
                if (sustainSamplesCounter < sustainTimeSamples)
                {
                    currentEnvelope = grainAmplitude;
                    ++sustainSamplesCounter;
                }
                else
                {
                    sustainSamplesCounter = 0;
                    state = RELEASE;
                }
                break;

            case RELEASE:
                if (releaseSamplesCounter < releaseTimeSamples)
                {
                    currentEnvelope = (1.f + std::cos(M_PI * (releaseSamplesCounter/releaseTimeSamples)))*(grainAmplitude/2.f);
                    ++releaseSamplesCounter;
                }
                else
                {
                    releaseSamplesCounter = 0;
                    state = OFF;
                }
                break;
            
            default:
                break;
            }

            output[n] = currentEnvelope;
        }
    }
}