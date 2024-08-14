#include "Led.h"

//==============================================================================
Led::Led() 
{
    ledColour = juce::Colours::darkblue;
}

void Led::paint(juce::Graphics& g) 
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    juce::ColourGradient gradient(ledColour.brighter(0.2f), bounds.getCentreX(), bounds.getCentreY(),
                                  ledColour.darker(0.2f), bounds.getRight(), bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillEllipse(bounds);
    g.setColour(juce::Colours::black);
    g.drawEllipse(bounds, 1.0f);
    g.setColour(juce::Colours::white.withAlpha(1.0f));
    g.fillEllipse(bounds.reduced(bounds.getWidth() * 0.7f));
    }

//==============================================================================
void Led::setLedOn(bool isOn)
{
    ledColour = isOn ? juce::Colours::blue : juce::Colours::darkblue;
    repaint();
}
