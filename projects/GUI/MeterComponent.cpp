#include "MeterComponent.h"

namespace GUI
{

MeterComponent::MeterComponent(DSP::Meter& m) :
    meter(m)
{
    startTimerHz(60);
}

MeterComponent::~MeterComponent()
{
}

void MeterComponent::resized()
{
    auto bounds = getLocalBounds();
    grad = juce::ColourGradient::vertical(juce::Colours::yellow, juce::Colours::green, bounds);
}

void MeterComponent::paint(juce::Graphics& g)
{
    unsigned int numChannels = meter.getNumChannels();

    float leftEnvelope = meter.getEnvelope(0);
    float leftEnvelopeDb = 20.f * std::log10(std::fmax(leftEnvelope, 1e-6));

    float rightEnvelope = leftEnvelope;
    float rightEnvelopeDb = leftEnvelopeDb;

    juce::Rectangle<int> totalArea = getLocalBounds();
    juce::Rectangle<int> leftChannelArea;
    juce::Rectangle<int> rightChannelArea;
    if (numChannels > 1)
    {
        rightEnvelope = meter.getEnvelope(1);
        rightEnvelopeDb = 20.f * std::log10(std::fmax(rightEnvelope, 1e-6));

        leftChannelArea = totalArea.removeFromLeft(getWidth() / 2);
        rightChannelArea = totalArea;

        float leftFillPropHeight = (leftEnvelopeDb - MIN_DB_SCALE) / (MAX_DB_SCALE - MIN_DB_SCALE);
        float rightFillPropHeight = (rightEnvelopeDb - MIN_DB_SCALE) / (MAX_DB_SCALE - MIN_DB_SCALE);

        juce::Rectangle<float> leftProp(0.f, 1.f - leftFillPropHeight,
                                        1.f, leftFillPropHeight);
        juce::Rectangle<float> rightProp(0.f, 1.f - rightFillPropHeight,
                                         1.f, rightFillPropHeight);

        if (leftEnvelopeDb >= 0.f)
            g.setColour(juce::Colours::red);
        else
            g.setGradientFill(grad);
        g.fillRect(leftChannelArea.getProportion(leftProp));

        if (rightEnvelopeDb >= 0.f)
            g.setColour(juce::Colours::red);
        else
            g.setGradientFill(grad);
        g.fillRect(rightChannelArea.getProportion(rightProp));
    }
    else
    {
        leftChannelArea = totalArea;

        float leftFillPropHeight = (leftEnvelopeDb - MIN_DB_SCALE) / (MAX_DB_SCALE - MIN_DB_SCALE);

        juce::Rectangle<float> leftProp(0.f, 1.f - leftFillPropHeight,
                                        1.f, leftFillPropHeight);

        g.setColour(juce::Colours::green);
        g.fillRect(leftChannelArea.getProportion(leftProp));
    }
}

void MeterComponent::timerCallback()
{
    repaint();
}

}
