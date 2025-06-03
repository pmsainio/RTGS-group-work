#include <JuceHeader.h>
#include "PluginEditor.h"


GrainAudioProcessorEditor::GrainAudioProcessorEditor(GrainAudioProcessor& p) :
    juce::AudioProcessorEditor(p), audioProcessor(p),
    paramEditor(audioProcessor.getParamManager())
{
    addAndMakeVisible(paramEditor);
    getLookAndFeel().setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::black);
    setSize(6 * 100, 6 * 100);
}

GrainAudioProcessorEditor::~GrainAudioProcessorEditor()
{
}

//==============================================================================
void GrainAudioProcessorEditor::paint(juce::Graphics& g)
{
     g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

}

void GrainAudioProcessorEditor::resized()
{
    paramEditor.setBounds(getLocalBounds());
}