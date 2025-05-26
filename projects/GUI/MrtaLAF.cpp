#include "MrtaLAF.h"

namespace GUI
{

MrtaLAF::MrtaLAF() :
    grad(juce::Colours::yellow, juce::Point<float>(0.5f, 50.f), juce::Colours::green, juce::Point<float>(0.5f, 0.f), false)
{
}

MrtaLAF::~MrtaLAF()
{
}

void MrtaLAF::drawRotarySlider(Graphics& g, int x, int y, int width, int height,
                               float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                               Slider&)
{
    // total area of out slider
    juce::Rectangle<float> area(x, y, width, height);

    // proportional rect of slider position
    juce::Rectangle<float> fillProp(0.25f, 1.f - sliderPosProportional,
                                    0.5f, sliderPosProportional);

    // proportional area of outline
    juce::Rectangle<float> outlineProp(0.25f, 0.f, 0.5f, 1.f);

    // fill slider position rect
    g.setGradientFill(grad);
    g.fillRoundedRectangle(area.getProportion(fillProp), 5.f);

    // draw slider outline rect
    g.setColour(juce::Colours::darkgrey);
    g.drawRoundedRectangle(area.getProportion(outlineProp),
                           5.f, 4.f);
}

void MrtaLAF::drawButtonBackground(juce::Graphics& g, juce::Button& b, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    if (b.getToggleState())
    {
        g.setColour(juce::Colours::green.darker(shouldDrawButtonAsDown ? 0.6f : 0.f));
    }
    else
    {
        g.setColour(juce::Colours::red.darker(shouldDrawButtonAsDown ? 0.6f : 0.f));
    }

    g.fillRoundedRectangle(b.getLocalBounds().toFloat(), 5.f);

    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(b.getLocalBounds().toFloat(), 5.f, 3.f);
}

void MrtaLAF::drawButtonText(juce::Graphics& g, juce::TextButton& b, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    g.setColour(juce::Colours::grey);
    g.setFont(juce::FontOptions(25.f));
    g.drawText(b.getButtonText(), b.getLocalBounds(), juce::Justification::centred);
}

}
