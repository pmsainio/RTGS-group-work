#include "SequenceScheduler.h"
#include <cstdlib>

namespace DSP {

    SequenceScheduler::SequenceScheduler(float minLength, float maxLength, int fileLength, double sampleRate) {}

    float SequenceScheduler::frandom()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    float SequenceScheduler::nextDuration()
    {
        return static_cast<int>((minLength + frandom() * (maxLength - minLength)) * sampleRate);
    }

std::vector<int> SequenceScheduler::generateStartingPoints(float maxLength, int fileLength)
{
    std::vector<int> startingPoints;
    float currentPosition = 0.0f;

    while (currentPosition < fileLength)
    {
        float duration = nextDuration();
        if (currentPosition + duration > fileLength)
            break; // Prevent going out of bounds

        startingPoints.push_back(currentPosition);
        currentPosition += duration;
    }

    return startingPoints;

}

} // namespace DSP