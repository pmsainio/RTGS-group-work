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
            void setPitchRatio(float ratio) { pitchRatio = ratio; };
            bool trigger(int startPos, int endPos, float pitchRatio);

            void setGrainEnv(float attack, float sustain, float release);
            void setGrainAmp(float amplitude) { grainSustain = amplitude; };

            int getActiveGrainCount() const;

        private:
            struct ActiveGrain {
                std::unique_ptr<GrainEnvelope> envelope { std::make_unique<GrainEnvelope>() }; 
                int currPos = 0;
                int startPos = 0;
                int endPos = 0;
                bool active = true; 
                float amplitude = 0.f;
                juce::AudioBuffer<float> envelopeBuffer;
                float currPosFloat = 0.f;
                float pitchRatio = 0.f;

                ActiveGrain() = default;
            };

            float frandom();
            double randomDuration(float minLength, float maxLength);

            double sampleRate {48000.0};
            int fileLength = 0;

            float grainAttack = 10.f; 
            float grainRelease = 10.f;
            float grainSustain = 0.1f;
            float grainAmplitude = 1.f;
            float nextGrainTime = 0.f;
            float pitchRatio = 0.f;

            juce::AudioBuffer<float>* sampleBuffer = nullptr;
            std::vector<ActiveGrain> grains;
    };
} // namespace DSP
