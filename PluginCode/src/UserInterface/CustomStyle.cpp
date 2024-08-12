#include "CustomStyle.h"

//==============================================================================
CustomStyle::CustomStyle()
{
}

//==============================================================================
void CustomStyle::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                  const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    juce::ignoreUnused(slider);

    float diameter = 0.75f * float(fmin(width, height));
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
    knobTick.addRectangle(-0.5f * tickWidth, -0.8f * radius, tickWidth, 0.6f * radius);
    g.setColour(juce::Colours::whitesmoke);
    g.fillPath(knobTick, juce::AffineTransform::rotation(angle).translated(centerX, centerY));   

    juce::Path arcPath;
    float arcRadius = radius + 5.0f; 
    arcPath.addArc(centerX - arcRadius, centerY - arcRadius, 2.0f * arcRadius, 2.0f * arcRadius, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colours::black);
    g.strokePath(arcPath, juce::PathStrokeType(3.0f));

    juce::Path arrow;
    float arrowLength = 5.0f;
    float arrowWidth = 3.0f;

    // Calcola il punto finale dell'arco
    juce::Point<float> endPoint(centerX + arcRadius * std::cos(-0.25f * mathConst::pi),
                                centerY + arcRadius * std::sin(0.25f * mathConst::pi));

    // Calcola i punti della freccia
    juce::Point<float> basePoint1(endPoint.x - arrowLength * std::cos(rotaryEndAngle - juce::MathConstants<float>::halfPi),
                                  endPoint.y - arrowLength * std::sin(rotaryEndAngle - juce::MathConstants<float>::halfPi));
    juce::Point<float> basePoint2(endPoint.x - arrowLength * std::cos(rotaryEndAngle + juce::MathConstants<float>::halfPi),
                                  endPoint.y - arrowLength * std::sin(rotaryEndAngle + juce::MathConstants<float>::halfPi));

    g.setColour(juce::Colours::red);
    g.fillEllipse(endPoint.x - 2.0f, endPoint.y - 2.0f, 4.0f, 4.0f);  // Punto finale dell'arco
    g.fillEllipse(basePoint1.x - 2.0f, basePoint1.y - 2.0f, 4.0f, 4.0f);  // Base punto 1
    g.fillEllipse(basePoint2.x - 2.0f, basePoint2.y - 2.0f, 4.0f, 4.0f);  // Base punto 2

    // Aggiungi il triangolo alla Path
    arrow.addTriangle(endPoint, basePoint1, basePoint2);

    // Disegna la freccia
    g.setColour(juce::Colours::black);
    g.fillPath(arrow);
}

void CustomStyle::drawLabel(juce::Graphics& g, juce::Label& label)
{
    juce::Font comicSansFont("Comic Sans MS", 20.0f, juce::Font::plain);
    g.setFont(comicSansFont);
    g.setColour(juce::Colours::black);
    g.drawFittedText(label.getText(), label.getLocalBounds(), juce::Justification::centred, 1);
}
