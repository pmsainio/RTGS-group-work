#include <JuceHeader.h>
#include "PluginEditor.h"


GrainAudioProcessorEditor::GrainAudioProcessorEditor(GrainAudioProcessor& p) :
    juce::AudioProcessorEditor(p), audioProcessor(p),
    paramEditor(audioProcessor.getParamManager()),
    loadButton("Load Audio File")
{
    audioProcessor.addChangeListener(this);
    addAndMakeVisible(paramEditor);

    addAndMakeVisible(rangeSlider);
    rangeSlider.juce::Slider::setSliderStyle(juce::Slider::SliderStyle::TwoValueHorizontal);
    rangeSlider.setRange(30, 1000); // desired range
    rangeSlider.setMinAndMaxValues(30, 130); // initial min/max values
    
    rangeSlider.onValueChange = [this] {
        // handle value changes here
        auto min = rangeSlider.getMinValue();
        auto max = rangeSlider.getMaxValue();
        // update parameters as needed
    };

    addAndMakeVisible(loadButton);
    loadButton.addListener(this);
    loadButton.setColour(juce::TextButton::buttonColourId, juce::Colours::whitesmoke);
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::fuchsia);

    getLookAndFeel().setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::black);
    setSize(7 * 100, 7 * 100);
}

GrainAudioProcessorEditor::~GrainAudioProcessorEditor()
{
    audioProcessor.removeChangeListener(this);
}

//==============================================================================
void GrainAudioProcessorEditor::paint(juce::Graphics& g)
{
     g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

     if (fileLoaded)
     {
        auto waveformArea = getLocalBounds().removeFromTop(getHeight() * 0.2f).reduced(10);
        g.setColour(juce::Colours::fuchsia.withAlpha(0.8f));
        g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
        
        // Draw border
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawRect(waveformArea, 1);
     }
}

void GrainAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // allocate space for the slider
    auto sliderArea = bounds.removeFromBottom(50);
    rangeSlider.setBounds(sliderArea.reduced(10));

    auto buttonArea = bounds.removeFromTop(40).reduced(10);
    loadButton.setBounds(buttonArea);

    auto waveformArea = bounds.removeFromTop(getHeight() * 0.1f).reduced(10);
    
    // give the remaining space to the parameter editor
    paramEditor.setBounds(bounds);

    if (fileLoaded)
    {
        generateWaveform();
    }
}

void GrainAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        // Create file chooser
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file to load...",
            juce::File{},  // Start from default directory
            "*.wav;*.aiff;*.flac;*.mp3;*.ogg",  // Supported file formats
            true);  // Allow multiple files to be selected
        
        // Launch the file chooser
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | 
                                juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& chooser)
            {
                auto result = chooser.getURLResult();
                
                if (result.isEmpty())
                    return;
                
                juce::String path;
                if (result.isLocalFile())
                {
                    path = result.getLocalFile().getFullPathName();
                }
                else
                {
                    DBG("Remote files not supported yet");
                    return;
                }
                
                audioProcessor.readFile(path);
                
                loadButton.setButtonText(result.getFileName());
                fileLoaded = audioProcessor.getAudioBuffer() != nullptr;
                generateWaveform();
                repaint();
            });
    }
}

void GrainAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &audioProcessor)
    {
        fileLoaded = audioProcessor.getAudioBuffer() != nullptr;
        generateWaveform();
        repaint();
    }
}

void GrainAudioProcessorEditor::generateWaveform()
{
    waveformPath.clear();
    
    const auto* buffer = audioProcessor.getAudioBuffer();
    if (buffer == nullptr || buffer->getNumSamples() == 0)
        return;
    
    const float* channelData = buffer->getReadPointer(0);
    const int numSamples = buffer->getNumSamples();
    const int width = getWidth();
    
    // Get the waveform display area bounds
    auto waveformArea = getLocalBounds().removeFromTop(getHeight() * 0.2f).reduced(10);
    const int displayWidth = waveformArea.getWidth();
    
    // To avoid drawing too many points, we'll skip samples when zoomed out
    const int step = juce::jmax(1, numSamples / displayWidth);
    
    waveformPath.startNewSubPath(waveformArea.getX(), waveformArea.getCentreY());
    
    for (int x = 0; x < displayWidth; ++x)
    {
        const int sampleIndex = (int)((x / (float)displayWidth) * numSamples);
        const float sample = channelData[sampleIndex];
        
        const float y = juce::jmap(sample, -1.0f, 1.0f, 
                                 (float)waveformArea.getBottom(), 
                                 (float)waveformArea.getY());
        
        if (x == 0)
            waveformPath.startNewSubPath(waveformArea.getX() + (float)x, y);
        else
            waveformPath.lineTo(waveformArea.getX() + (float)x, y);
    }
}