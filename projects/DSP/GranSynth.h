#pragma once 

#include "GrainEnvelope.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include <vector>

namespace DSP
{   
    class GranSynth
    {
        public: 
            struct GrainParams
            {
                int startPos = 0; 
                int endPos = 0;
                float amplitude = 1.0f;
                bool active = false;
            };

            // CTOR & DTOR
            GranSynth(double sampleRate);
            ~GranSynth();

            // Semantics
            GranSynth(GranSynth&&) noexcept;
            GranSynth& operator=(GranSynth&&) noexcept;
            GranSynth(const GranSynth&) = delete;
            GranSynth& operator=(const GranSynth&) = delete;

            void setBuffer(juce::AudioBuffer<float>* buffer);
            
            void prepare(double newSampleRate);
            void processBlock(juce::AudioBuffer<float>& outputBuffer);

            void synthesize(float density, float minSize, float maxSize);

            void setGrainEnv(float attack, float sustain, float release);
            void setGrainAmp(float amplitude);

        private:
            struct ActiveGrain
            {
                std::unique_ptr<GrainEnvelope> envelope;
                int currPos = 0;
                int startPos = 0;
                int endPos = 1000;
                bool active = true; 
                float amplitude = 0.f;
                juce::AudioBuffer<float> envelopeBuffer;
                
                ActiveGrain() = default;
            };

            void trigger(int startPos, int endPos);
            float frandom();
            double randomDuration(float minLength, float maxLength);

            double sampleRate {48000.0};
            int fileLength = 0;

            float grainAttack = 5.f; 
            float grainRelease = 5.f;
            float grainSustain = 25.f;
            float grainAmplitude = 1.f;
            float nextGrainTime = 0.f;

            juce::AudioBuffer<float>* sampleBuffer = nullptr;
            std::vector<ActiveGrain> grains;
    };
} // namespace DSP
