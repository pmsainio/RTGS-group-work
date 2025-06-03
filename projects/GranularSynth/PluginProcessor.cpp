#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "juce_audio_formats/juce_audio_formats.h"
#include <JuceHeader.h>

static const std::vector<mrta::ParameterInfo> paramVector 
{
    { Param::ID::volume, Param::Name::volume, "", 0.5f, Param::Ranges::volumeMin, Param::Ranges::volumeMax, 0.01f, 1.0f }
};

GrainAudioProcessor::GrainAudioProcessor()
    : paramManager(*this, "GranularSynth", paramVector)
{
    paramManager.registerParameterCallback(Param::ID::volume,
    [this] (float value, bool /*force*/)
    {
        DBG(value);
    });

}

GrainAudioProcessor::~GrainAudioProcessor()
{
}

void GrainAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate; 
    formatManager.registerBasicFormats();

    juce::String fileToLoad;
    readFile(fileToLoad);
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
        juce::AudioBuffer<float> fileBuffer((int)reader->numChannels, (int)reader->lengthInSamples);
        reader->read(&fileBuffer, 0, (int)reader->lengthInSamples, 0, true, true);

        DBG("Audio Read:" + file.getFileName());
    }
    else
    {
        DBG("Failed to create reader for file");
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

void GrainAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const int numSamples = buffer.getNumSamples();
    auto* output = buffer.getWritePointer(0);
    
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