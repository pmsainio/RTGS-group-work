#pragma once 

#include "GrainEnvelope.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include <vector>

namespace DSP
{   
    class GranSynth
    {
        public: 
            // GrainParams struct, this makes it a lot neater since each Grain will always own these params. 
            // It is a lot more efficient than assigning a variable to each grain, which WILL get overwritten if done that way. 
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

            /* Buffer is set, which is again, a pointer to the buffer. This is the pointing to the same buffer 
            as GrainSynthVoice, so no new copies are made. */
            void setBuffer(juce::AudioBuffer<float>* buffer);
            
            void prepare(double newSampleRate);
            
            /* Only processes audio, has nothing to do with grain logic. Points to the output buffer,
            which is called in the blockprocess method of GrainSynthVoice */
            void processBlock(juce::AudioBuffer<float>& outputBuffer);

            /* 1. Synthesize "makes the grains" based on the parameters given to it. 
               2. Trigger only gives the PARAMS to each grain, does nothing with synthesis, it is only telling synthesize 
                  that a grain has been requested. 
               3. PitchRatio is set based on the note pressed. */
            void synthesize(float density, float minSize, float maxSize);
            void setPitchRatio(float ratio) { pitchRatio = ratio; };
            bool trigger(int startPos, int endPos, float pitchRatio);

            // Grain Envelope is only set here so that it can be passed on to the GrianEnv class. 
            void setGrainEnv(float attack, float sustain, float release);

            // Tells how many grains are active so we can better manage grain lifecycle. 
            int getActiveGrainCount() const;

        private:
            /* When a grian is triggered, it is made into an *active* grain, which means that it is now officially a grain
               and takes the following paramters. */
            struct ActiveGrain {
                // Each grain is given a poniter to the envelope object. 
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

            // Randomization functions 
            float frandom();
            double randomDuration(float minLength, float maxLength);

            double sampleRate {48000.0};
            int fileLength = 0;

            // Grain Params
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
