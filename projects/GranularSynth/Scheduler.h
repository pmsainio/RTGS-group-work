#ifdef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "SequenceStrategy.h"

class Scheduler {
private:
    int nextOnset;
    SequenceStrategy sequenceStrategy;
    std::vector<float> activeGrains;
public:
    Scheduler(float minIO, float maxIO, float density);

    void activateGrain(float duration);
    float synthesizeActiveGrains();
    float synthesize(bool useDensityBasedInteronset = false);
};

#endif // SCHEDULER_H