#ifndef SEQUENCE_STRATEGY_H
#define SEQUENCE_STRATEGY_H

#include <cmath>
#include <cstdlib>

class SequenceStrategy {
    public:
        float nextDuration();
        float nextInteronset();
};

class Scheduler {
    private:
        float minInteronset;
        float maxInteronset;

    public:
        Scheduler(float minIO, float maxIO);

        float frandom();
        float nextDuration();
        float nextInteronset();
};

#endif // SEQUENCE_STRATEGY_H
