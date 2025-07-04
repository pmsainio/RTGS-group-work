#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "juce_audio_formats/juce_audio_formats.h"
#include "mrta_utils/Source/Parameter/ParameterManager.h"
#include <JuceHeader.h>
#include <memory>

static const std::vector<mrta::ParameterInfo> paramVector 
{   
    // {ID, Name, Units, Default, Min, Max, Incr, Skew}
    { Param::ID::filePos, Param::Name::filePos, "Samples", 0.f, 0.f, 1.f, 0.001f, 1.f },
    { Param::ID::grainLen, Param::Name::grainLen, "ms", 30.f, 30.f, 200.f, 1.f, 1.f },
    { Param::ID::density, Param::Name::density, "Samples", 5.f, 5.f, 20.f, 1.f, 1.f },
    { Param::ID::attack, Param::Name::attack, "ms", 10.f, 10.f, 25.f, 1.f, 1.0f },
    { Param::ID::release, Param::Name::release, "ms", 10.f, 10.f, 25.f, 1.f, 1.0f },
    { Param::ID::sustain, Param::Name::sustain, "", 0.1f, 0.1f, 1.f, 0.1f, 1.0f },
    { Param::ID::minGrainLen, Param::Name::minGrainLen, "ms", 50.f, 50.f, 200.f, 1.f, 1.f },
    { Param::ID::maxGrainLen, Param::Name::maxGrainLen, "ms", 30.f, 30.f, 100.f, 1.f, 1.f }
};

GrainAudioProcessor::GrainAudioProcessor()
    : paramManager(*this, "GranularSynth", paramVector)
{   

    // Register all audio formats and make new object for GrainSynthVoice and pass the object reference to GranSynth
    formatManager.registerBasicFormats();
    granSynth = std::make_unique<DSP::GranSynth>(sampleRate); 
    voice = new GrainSynthVoice();
    synth.addSound(new GrainSynthSound());
    synth.addVoice(voice);
    voice->setGranSynth(granSynth.get()); 

    // Param Read.
    paramManager.registerParameterCallback(Param::ID::attack,
        [this](float value, bool /*force*/) {
            voice->setGrainAttack(value);
        });

    paramManager.registerParameterCallback(Param::ID::sustain,
        [this](float value, bool /*force*/) {
            voice->setGrainSustain(value);
        });

    paramManager.registerParameterCallback(Param::ID::release,
        [this](float value, bool /*force*/) {
            voice->setGrainRelease(value);
        });

    paramManager.registerParameterCallback(Param::ID::filePos,
        [this](float value, bool /*force*/) {
            voice->setFilePos(value);
        });

    paramManager.registerParameterCallback(Param::ID::minGrainLen,
        [this](float value, bool /*force*/) {
            voice->setMinSize(value);
        });
    
    paramManager.registerParameterCallback(Param::ID::maxGrainLen,
        [this](float value, bool /*force*/) {
            voice->setMaxSize(value);
        });

    paramManager.registerParameterCallback(Param::ID::density,
        [this](float value, bool /*force*/) {
            voice->setDensity(value);
        });

}

GrainAudioProcessor::~GrainAudioProcessor()
{
}

void GrainAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate; 

    // Checks object exists before preparing the GranSynth class. 
    if (granSynth) {
        granSynth->prepare(sampleRate);
    }

    paramManager.updateParameters(true);

    // Initialize the synth
    synth.setCurrentPlaybackSampleRate(sampleRate);

    checkForRestoredPath();
}

void GrainAudioProcessor::readFile(juce::String path)
{   
    // File Handling. Loads file and does some basic error checking to ensure that the file exists. 

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
        notifyWaveformChanged();

        DBG("Audio Read:" + file.getFileName());
    }
    else
    {
        DBG("Failed to create reader for file");
    }

    if (fileBuffer) {
    DBG("Buffer created - Channels: " << fileBuffer->getNumChannels() 
        << ", Samples: " << fileBuffer->getNumSamples());
        
    // Quick content check and level check. 
    float rms = 0;
    for (int i = 0; i < fileBuffer->getNumSamples(); i += 100) {
        rms += std::abs(fileBuffer->getSample(0, i));
    }
    DBG("Approx RMS: " << (rms / (fileBuffer->getNumSamples()/100.f)));
}
}

void GrainAudioProcessor::checkForRestoredPath()
{   
    // This function basically checks if there is a restored path, barely ever gets called.
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
    paramManager.updateParameters();
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
    paramManager.getStateInformation(destData);
}

void GrainAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    paramManager.setStateInformation(data, sizeInBytes);
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