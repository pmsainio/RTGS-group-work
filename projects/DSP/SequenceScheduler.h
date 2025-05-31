#pragma once 

#include <vector>

namespace DSP
{   
    class SequenceScheduler
    {
        public: 
            // CTOR & DTOR
            SequenceScheduler(float minIO, float maxIO, float density);
            ~SequenceScheduler();

            // Semantics
            SequenceScheduler(SequenceScheduler&&) noexcept;
            SequenceScheduler& operator=(SequenceScheduler&&) noexcept;
            SequenceScheduler(const SequenceScheduler&) = delete;
            SequenceScheduler& operator=(const SequenceScheduler&) = delete;

            // Sequence Strat Methods
            void activateGrain(float duration);
            float synthesizeActiveGrain();
            
            // Scheduler Methods
            float frandom();
            void scheduleSynthesize(float* out, unsigned int len);
            float nextDuration();
            float nextInteronset();
        private: 
            // Scheduler Params
            std::vector<float> activeGrains;
            int nextOnset;

            // Sequence Strat Params
            float minInteronset;
            float maxInteronset;
            float density;
    };
} // namespace DSP
