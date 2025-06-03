#include "PluginEditor.h"
#include "mrta_utils/Source/GUI/GenericParameterEditor.h"

GrainAudioProcessorEditor::GrainAudioProcessorEditor(GrainAudioProcessor& p) :
    juce::AudioProcessorEditor(p), audioProcessor(p),
    paramEditor(audioProcessor.getParamManager())
{
    addAndMakeVisible(paramEditor);
    setSize(300, 300);
}


GrainAudioProcessorEditor::~GrainAudioProcessorEditor()
{
}

//==============================================================================
void GrainAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void GrainAudioProcessorEditor::resized()
{
    paramEditor.setBounds(getLocalBounds());
}