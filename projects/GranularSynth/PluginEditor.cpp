#include <JuceHeader.h>
#include "PluginEditor.h"


GrainAudioProcessorEditor::GrainAudioProcessorEditor(GrainAudioProcessor& p) :
    juce::AudioProcessorEditor(p), audioProcessor(p),
    paramEditor(audioProcessor.getParamManager())
{
    addAndMakeVisible(paramEditor);

    addAndMakeVisible(rangeSlider);
    rangeSlider.juce::Slider::setSliderStyle(juce::Slider::SliderStyle::TwoValueHorizontal);
    rangeSlider.setRange(0.0, 1.0); // desired range
    rangeSlider.setMinAndMaxValues(0.2, 0.8); // initial min/max values
    
    rangeSlider.onValueChange = [this] {
        // handle value changes here
        auto min = rangeSlider.getMinValue();
        auto max = rangeSlider.getMaxValue();
        // update parameters as needed
    };

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
    auto bounds = getLocalBounds();
    
    // allocate space for the slider
    auto sliderArea = bounds.removeFromBottom(50);
    rangeSlider.setBounds(sliderArea.reduced(10));
    
    // give the remaining space to the parameter editor
    paramEditor.setBounds(bounds);
}