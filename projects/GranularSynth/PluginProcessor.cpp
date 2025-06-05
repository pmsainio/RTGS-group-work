#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "juce_audio_formats/juce_audio_formats.h"
#include "mrta_utils/Source/Parameter/ParameterManager.h"
#include <JuceHeader.h>
#include <memory>

static const std::vector<mrta::ParameterInfo> paramVector 
{
    { Param::ID::volume, Param::Name::volume, "", 0.1f, Param::Ranges::volumeMin, Param::Ranges::volumeMax, 0.01f, 0.3f },
    { Param::ID::attack, Param::Name::attack, "ms", 5.f, Param::Ranges::attackMin, Param::Ranges::attackMax, 0.1f, 1.0f },
    { Param::ID::release, Param::Name::release, "ms", 5.f, Param::Ranges::relMin, Param::Ranges::relMax, 0.1f, 1.0f },
    { Param::ID::sustain, Param::Name::sustain, "ms", 0.01f, Param::Ranges::susMin, Param::Ranges::susMax, 0.1f, 1.0f },
    { Param::ID::filePos, Param::Name::filePos, "Samples", 0.f, 0.f, 1.f, 0.001f, 1.f },
    { Param::ID::grainLen, Param::Name::grainLen, "Samples", 480.f, Param::Ranges::grainSizeMin, Param::Ranges::grainSizeMax, 1.f, 1.f }  
};

GrainAudioProcessor::GrainAudioProcessor()
    : paramManager(*this, "GranularSynth", paramVector)
{

    formatManager.registerBasicFormats();
    granSynth = std::make_unique<DSP::GranSynth>(sampleRate); 
    voice = new GrainSynthVoice();
    synth.addSound(new GrainSynthSound());
    synth.addVoice(voice);
    voice->setGranSynth(granSynth.get()); 
    
       paramManager.registerParameterCallback(Param::ID::volume,
        [this](float value, bool /*force*/) {
            if (voice) {
                voice->setGrainAmp(value);
                if (auto* granSynth = voice->getGranSynth()) {
                    granSynth->setGrainAmp(value);
                }
            }
        });

    paramManager.registerParameterCallback(Param::ID::attack,
        [this](float value, bool /*force*/) {
            if (voice) {
                voice->setGrainAttack(value);
                if (auto* granSynth = voice->getGranSynth()) {
                    granSynth->setGrainEnv(value, 
                                        voice->getGrainSustain(), 
                                        voice->getGrainRelease());
                }
            }
        });

    paramManager.registerParameterCallback(Param::ID::sustain,
        [this](float value, bool /*force*/) {
            if (voice) {
                voice->setGrainSustain(value);
                if (auto* granSynth = voice->getGranSynth()) {
                    granSynth->setGrainEnv(voice->getGrainAttack(), 
                                         value, 
                                         voice->getGrainRelease());
                }
            }
        });

    paramManager.registerParameterCallback(Param::ID::release,
        [this](float value, bool /*force*/) {
            if (voice) {
                voice->setGrainRelease(value);
                if (auto* granSynth = voice->getGranSynth()) {
                    granSynth->setGrainEnv(voice->getGrainAttack(), 
                                         voice->getGrainSustain(), 
                                         value);
                }
            }
        });

    paramManager.registerParameterCallback(Param::ID::grainLen,
        [this](float value, bool /*force*/) {
            if (voice) {
                voice->setGrainSize(value);
            }
        });

    paramManager.registerParameterCallback(Param::ID::filePos,
        [this](float value, bool /*force*/) {
            float grainRelativePos = value * paramManager.getAPVTS().getRawParameterValue(Param::ID::grainLen)->load();
            voice->setFilePosition(grainRelativePos);
        });
}

GrainAudioProcessor::~GrainAudioProcessor()
{
}

void GrainAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate; 

    if (granSynth) {
        granSynth->prepare(sampleRate);
    }

    // Initialize the synth
    synth.setCurrentPlaybackSampleRate(sampleRate);

    checkForRestoredPath();
}

void GrainAudioProcessor::readFile(juce::String path)
{   
    juce::File file(path);
    DBG("Trying to load file: " << path << "\n");
    if (!file.existsAsFile())
    {
        DBG("File doesn't exist");
        return;
    }
    
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader != nullptr)
    {   
        fileBuffer = std::make_unique<juce::AudioBuffer<float>>((int)reader->numChannels, (int)reader->lengthInSamples);
        reader->read(fileBuffer.get(), 0, (int)reader->lengthInSamples, 0, true, true);

        if (voice) {
            voice->setSampleBuffer(fileBuffer.get());
        }

        DBG("Audio Read:" + file.getFileName());
    }
    else
    {
        DBG("Failed to create reader for file");
    }

    if (fileBuffer) {
    DBG("Buffer created - Channels: " << fileBuffer->getNumChannels() 
        << ", Samples: " << fileBuffer->getNumSamples());
        
    // Quick content check
    float rms = 0;
    for (int i = 0; i < fileBuffer->getNumSamples(); i += 100) {
        rms += std::abs(fileBuffer->getSample(0, i));
    }
    DBG("Approx RMS: " << (rms / (fileBuffer->getNumSamples()/100)));
}
}

void GrainAudioProcessor::checkForRestoredPath()
{
    juce::String path;
    path = restoredPath; 
    if (path.isNotEmpty())
    {
        DBG("Restored Path:" << path);
        std::swap(chosenPath, path);
        restoredPath = "";
    }
}

void GrainAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const int numSamples = buffer.getNumSamples();
    auto* output = buffer.getWritePointer(0);
    synth.renderNextBlock(buffer, midiMessages, 0, numSamples);
}

bool GrainAudioProcessor::acceptsMidi() const
{
    return true;
}

void GrainAudioProcessor::releaseResources()
{
}

void GrainAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{

}

void GrainAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{

}

//==============================================================================
const juce::String GrainAudioProcessor::getName() const { return JucePlugin_Name; }
bool GrainAudioProcessor::producesMidi() const { return false; }
bool GrainAudioProcessor::isMidiEffect() const { return false; }
double GrainAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int GrainAudioProcessor::getNumPrograms() { return 1; }
int GrainAudioProcessor::getCurrentProgram() { return 0; }
void GrainAudioProcessor::setCurrentProgram(int) { }
const juce::String GrainAudioProcessor::getProgramName(int) { return {}; }
void GrainAudioProcessor::changeProgramName(int, const juce::String&) { }
bool GrainAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* GrainAudioProcessor::createEditor() { return new GrainAudioProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GrainAudioProcessor();
}