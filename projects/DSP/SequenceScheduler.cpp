#include "SequenceScheduler.h"

namespace DSP
{
    SequenceScheduler::SequenceScheduler(float minIO, float maxIO, float density) 
    {
        minIO = 0;
        maxIO = 100;
        density = 1;
    }

    float SequenceScheduler::frandom()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    float SequenceScheduler::nextDuration()
    {
        return minInteronset + frandom() * (maxInteronset - minInteronset);
    }

    float SequenceScheduler::nextInteronset()
    {
        return minInteronset + frandom() * (maxInteronset - minInteronset);
    }

    void SequenceScheduler::activateGrain(float duration)
    {
        activeGrains.push_back(duration);
    }

    float SequenceScheduler::synthesizeActiveGrain()
    {
        return activeGrains.empty() ? 0.0f : activeGrains.back();
    }

}