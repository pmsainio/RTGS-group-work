#include "SequenceScheduler.h"
#include <cstdlib>

namespace DSP {

    SequenceScheduler::SequenceScheduler(float minLength, float maxLength, int fileLength, double sampleRate) {}

    float SequenceScheduler::frandom()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    double SequenceScheduler::randomDuration(float minLength, float maxLength)
    {
        return (minLength + frandom() * (maxLength - minLength) * sampleRate);
    }

std::vector<int> SequenceScheduler::generateStartingPoints(float minLength, float maxLength, int fileLength)
{
    std::vector<int> startingPoints;
    float currentPosition = 0.0f;

    while (currentPosition < fileLength)
    {
        float duration = randomDuration(minLength, maxLength);
        if (currentPosition + maxLength * sampleRate > fileLength)
            break; // Prevent going out of bounds

        startingPoints.push_back(currentPosition);
        currentPosition += duration;
    }

    return startingPoints;
}

std::vector<float> SequenceScheduler::generateGrainLengths(float minLength, float maxLength)
{
  // generates 64 grain lengths
    std::vector<float> grainLengths;
    for (int i = 0; i < 64; ++i)
    {
        float length = randomDuration(minLength, maxLength);
        grainLengths.push_back(length);
    }
    return grainLengths;
}


} // namespace DSP