#include "Led.h"

//==============================================================================
Led::Led() 
{
    ledColour = juce::Colour::fromRGB(0, 0, 255);
}

//==============================================================================
void Led::paint(juce::Graphics& g) 
{
    auto bounds = getLocalBounds().toFloat().reduced(0.8f);

    auto white = juce::Colours::white;
    auto black = juce::Colours::black;
    
    juce::ColourGradient gradient(ledColour.brighter(0.4f), bounds.getCentreX(), bounds.getCentreY(),
                                  ledColour.darker(1.0f), bounds.getRight(), bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillEllipse(bounds);
    g.setColour(black);
    g.drawEllipse(bounds, 1.0f);
}

//==============================================================================
void Led::setLedOn(bool isOn)
{
    ledColour = isOn ? juce::Colours::darkblue.darker(0.55f) : juce::Colour::fromRGB(0, 0, 255);
    repaint();
}