#include <JuceHeader.h>
#include "PluginEditor.h"

GrainAudioProcessorEditor::GrainAudioProcessorEditor(GrainAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p), loadButton("Load Audio File")
{
    startTimerHz(30); 
    audioProcessor.addChangeListener(this);

    auto& apvts = audioProcessor.getParamManager().getAPVTS();

    //addLabeledKnob(grainSizeSlider, grainSizeLabel, "Grain Size", Param::ID::grainLen);
    addLabeledKnob(grainMaxSizeSlider, grainMaxSizeLabel, "Sustain Max Size", Param::ID::maxGrainLen);
    addLabeledKnob(filePosSlider, filePosLabel, "File Pos", Param::ID::filePos);
    addLabeledKnob(densitySlider, densityLabel, "Density", Param::ID::density);

    addLabeledKnob(attackSlider, attackLabel, "Attack", Param::ID::attack);
    addLabeledKnob(sustainSlider, sustainLabel, "Sustain", Param::ID::sustain);
    addLabeledKnob(releaseSlider, releaseLabel, "Release", Param::ID::release);

    //addAndMakeVisible(rangeSlider); don't need all this in the current version
    //rangeSlider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    //rangeSlider.setRange(30, 1000);
    //rangeSlider.setMinAndMaxValues(30, 200);
    //rangeSlider.onValueChange = [this]() {
    //    auto minValue = rangeSlider.getMinValue();
    //    auto maxValue = rangeSlider.getMaxValue();

    //    auto& apvts = audioProcessor.getParamManager().getAPVTS();
    //    if (auto* minGrainLen = apvts.getParameter(Param::ID::minGrainLen))
    //        minGrainLen->setValueNotifyingHost(minValue);
    //    if (auto* maxGrainLen = apvts.getParameter(Param::ID::maxGrainLen))
    //        maxGrainLen->setValueNotifyingHost(maxValue);
    //};

    addAndMakeVisible(loadButton);
    loadButton.addListener(this);

    getLookAndFeel().setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::black);
    setSize(700, 700);
}

GrainAudioProcessorEditor::~GrainAudioProcessorEditor()
{
    audioProcessor.removeChangeListener(this);
}

void GrainAudioProcessorEditor::addLabeledKnob(std::unique_ptr<mrta::ParameterSlider>& slider, juce::Label& label, const juce::String& text, const juce::String& paramID)
{
    auto& apvts = audioProcessor.getParamManager().getAPVTS();
    slider = std::make_unique<mrta::ParameterSlider>(paramID, apvts);
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(label);
    addAndMakeVisible(slider.get());
}

void GrainAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    auto waveformHeight = getHeight() * 0.2f;
    auto waveformArea = getLocalBounds().removeFromTop((int)waveformHeight).reduced(10);

    if (fileLoaded)
    {
        g.setColour(juce::Colours::fuchsia.withAlpha(0.8f));
        g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawRect(waveformArea, 1);

        // File Pos Line
        float filePos = 0.f;
        if (auto* p = audioProcessor.getParamManager().getAPVTS().getRawParameterValue(Param::ID::filePos))
            filePos = p->load(); // in range [0, 1]

        int x = waveformArea.getX() + static_cast<int>(filePos * waveformArea.getWidth());
        g.setColour(juce::Colours::white);
        g.drawLine((float)x, (float)waveformArea.getY(), (float)x, (float)waveformArea.getBottom(), 1.5f);
    }

    // Divider
    float midY = getHeight() * 0.55f;
    g.setColour(juce::Colours::grey);
    g.drawLine(10.0f, midY, getWidth() - 10.0f, midY, 1.0f);
}

void GrainAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(getHeight() * 0.2f); // waveform
    loadButton.setBounds(bounds.removeFromTop(40).reduced(5));
    //rangeSlider.setBounds(bounds.removeFromBottom(50).reduced(5));

    auto topRow = bounds.removeFromTop(bounds.getHeight() / 2);
    auto bottomRow = bounds;

    auto colW = getWidth() / 3;
    auto knobH = 80;

    auto layout = [](juce::Rectangle<int> row, juce::Label& label1, juce::Component& knob1,
                                         juce::Label& label2, juce::Component& knob2,
                                         juce::Label& label3, juce::Component& knob3)
    {
        auto col1 = row.removeFromLeft(row.getWidth() / 3);
        auto col2 = row.removeFromLeft(row.getWidth() / 2);
        auto col3 = row;

        auto layoutKnob = [](juce::Rectangle<int> col, juce::Label& label, juce::Component& knob)
        {
            auto labelHeight = 20;
            label.setBounds(col.removeFromTop(labelHeight));
            knob.setBounds(col.reduced(10));
        };

        layoutKnob(col1, label1, knob1);
        layoutKnob(col2, label2, knob2);
        layoutKnob(col3, label3, knob3);
    };

    layout(topRow, grainMaxSizeLabel, *grainMaxSizeSlider,
                   filePosLabel,   *filePosSlider,
                   densityLabel,   *densitySlider);

    layout(bottomRow, attackLabel,  *attackSlider,
                      sustainLabel, *sustainSlider,
                      releaseLabel, *releaseSlider);
}

void GrainAudioProcessorEditor::timerCallback()
{
    repaint(); 
}

void GrainAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.aiff;*.flac;*.mp3;*.ogg",
            true);

        fileChooser->launchAsync(juce::FileBrowserComponent::openMode |
                                 juce::FileBrowserComponent::canSelectFiles,
                                 [this](const juce::FileChooser& chooser)
                                 {
                                     auto result = chooser.getURLResult();
                                     if (result.isEmpty()) return;

                                     juce::String path;
                                     if (result.isLocalFile())
                                         path = result.getLocalFile().getFullPathName();
                                     else return;

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
    if (!buffer || buffer->getNumSamples() == 0)
        return;

    const float* channelData = buffer->getReadPointer(0);
    int numSamples = buffer->getNumSamples();
    auto waveformArea = getLocalBounds().removeFromTop(getHeight() * 0.2f).reduced(10);
    int displayWidth = waveformArea.getWidth();

    int step = juce::jmax(1, numSamples / displayWidth);
    waveformPath.startNewSubPath(waveformArea.getX(), waveformArea.getCentreY());

    for (int x = 0; x < displayWidth; ++x)
    {
        int sampleIndex = (int)((x / (float)displayWidth) * numSamples);
        float sample = channelData[sampleIndex];
        float y = juce::jmap(sample, -1.0f, 1.0f, (float)waveformArea.getBottom(), (float)waveformArea.getY());

        if (x == 0)
            waveformPath.startNewSubPath(waveformArea.getX() + (float)x, y);
        else
            waveformPath.lineTo(waveformArea.getX() + (float)x, y);
    }
}
