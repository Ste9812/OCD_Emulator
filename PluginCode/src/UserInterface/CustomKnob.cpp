#include "CustomKnob.h"

//==============================================================================
CustomKnob::CustomKnob()
{

}

//==============================================================================
void CustomKnob::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                  const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    float diameter = float(fmin(width, height)) * 0.8f;
    float radius = diameter / 2.0f;
    float centerX = float(x) + float(width) / 2.0f;
    float centerY = float(y) + float(height) / 2.0f;
    float axisX = centerX - radius;
    float axisY = centerY - radius;
    float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    float tickWidth = 5.0f;

    juce::Rectangle<float> knobArea(axisX, axisY, diameter, diameter);
    g.setColour(juce::Colours::black);
    g.fillEllipse(knobArea);

    juce::Path knobTick;
    g.setColour(juce::Colours::whitesmoke);
    knobTick.addRectangle(0.0f, tickWidth - radius, tickWidth, radius * 0.6f);
    g.fillPath(knobTick, juce::AffineTransform::rotation(angle).translated(centerX, centerY));   
}