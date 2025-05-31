#pragma once

#include "Scheduler.h"
#include "SequenceStrategy.h"
#include "Scheduler.h"
#include <JuceHeader.h>

namespace DSP{

class Granulator {
    public: 
        Granulator();
        ~Granulator();

        // Semantics 
        Granulator(const Granulator&);
        const Granulator& operator=(const Granulator&);
        Granulator(Granulator&&) = delete;
        const Granulator& operator=(Granulator&&) = delete;

        void prepare(float newSampleRate, unsigned int numChannels, int newBlockSize, float delaySize);
        void process(AudioSampleBuffer& outputBuffer, AudioSampleBuffer& fileBuffer, unsigned int numChannels, unsigned int numSamples, unsigned int numFileSamples, float time);

        inline float cubicInterp(float x, float y0, float y1, float y2, float y3);
    private:
        double sampleRate;
        Scheduler scheduler;
        SequenceStrategy sequenceStrategy;
};

}