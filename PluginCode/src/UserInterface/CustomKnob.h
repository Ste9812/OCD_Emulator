#pragma once
#include <JuceHeader.h>

class CustomKnob : public juce::LookAndFeel_V4
{
public:
    CustomKnob();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;

};