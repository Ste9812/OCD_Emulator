#pragma once
#include <JuceHeader.h>

using mathConst = juce::MathConstants<float>;

class Led : public juce::Component
{
public:
    Led();

    void paint(juce::Graphics& g) override;

    void setLedOn(bool isOn);

private:
    juce::Colour ledColour;
};