#pragma once
#include <JuceHeader.h>

class Led : public juce::Component
{
public:
    Led();

    void paint(juce::Graphics& g) override;

    void setLedOn(bool isOn);

private:
    juce::Colour ledColour;
};