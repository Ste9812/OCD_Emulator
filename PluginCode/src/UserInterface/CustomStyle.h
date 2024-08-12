#pragma once
#include <JuceHeader.h>

using mathConst = juce::MathConstants<float>;

class CustomStyle : public juce::LookAndFeel_V4
{
public:
    CustomStyle();

private:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
};