#pragma once

#include <JuceHeader.h>

namespace GUI
{

class MrtaLAF : public juce::LookAndFeel_V4
{
public:
    MrtaLAF();
    ~MrtaLAF();

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                          Slider&) override;

    void drawButtonBackground(juce::Graphics&, Button&, const Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics&, juce::TextButton&,
                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    

private:
    juce::ColourGradient grad;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MrtaLAF)
};

}
