#pragma once 

#include <vector>

namespace DSP
{   
    class SequenceScheduler
    {
        public: 
            // CTOR & DTOR
            SequenceScheduler(float minLength, float maxLength, int fileLengthm, double sampleRate);
            ~SequenceScheduler();

            // Semantics
            SequenceScheduler(SequenceScheduler&&) noexcept;
            SequenceScheduler& operator=(SequenceScheduler&&) noexcept;
            SequenceScheduler(const SequenceScheduler&) = delete;
            SequenceScheduler& operator=(const SequenceScheduler&) = delete;

            float frandom();
            float nextDuration();

            // call whenever grain lengths are updated
            std::vector<int> generateStartingPoints(float maxLength, int fileLength, double sampleRate);

        private:
            float minLength;
            float maxLength;
            float fileLength;
            double sampleRate;
    };
} // namespace DSP
