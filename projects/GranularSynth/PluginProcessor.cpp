#include "PluginProcessor.h"
#include "juce_audio_formats/juce_audio_formats.h"
#include <JuceHeader.h>

GrainAudioProcessor::GrainAudioProcessor()
{
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

void GrainAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    buffer.clear();

    const int numSamples = buffer.getNumSamples();
    auto* output = buffer.getWritePointer(0);
    
}

void GrainAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{

}

void GrainAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{

}