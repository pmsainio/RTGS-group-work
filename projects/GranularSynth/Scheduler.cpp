class Scheduler {
private:
    int nextOnset;
    SequenceStrategy sequenceStrategy;
    std::vector<float> activeGrains;

public:
    Scheduler(float minIO, float maxIO)
        : nextOnset(0), sequenceStrategy(minIO, maxIO) {}

    void activateGrain(float duration) {
        activeGrains.push_back(duration);
    }

    float synthesizeActiveGrains() {
        return activeGrains.empty() ? 0.0f : activeGrains.back();
    }

    float synthesize() {
        return synthesizeActiveGrains();
    }
};
