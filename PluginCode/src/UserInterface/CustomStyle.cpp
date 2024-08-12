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
    float arcRadius = radius + 6.0f; 
    float arrowDegree = 10.0f / 180.0f * mathConst::pi;
    arcPath.addArc(centerX - arcRadius, centerY - arcRadius, 2.0f * arcRadius, 2.0f * arcRadius, rotaryStartAngle, rotaryEndAngle - arrowDegree, true);
    g.setColour(juce::Colours::black);
    g.strokePath(arcPath, juce::PathStrokeType(3.0f));

    juce::Path arrow;
    float arrowWidth = 8.0f;
    float endPointX = centerX + arcRadius * std::cos(0.25f * mathConst::pi + 0.45f * arrowDegree);
    float endPointY = centerY + arcRadius * std::sin(0.25f * mathConst::pi + 0.45f * arrowDegree);
    float basePointX1 = centerX + ( arcRadius + 0.5f * arrowWidth ) * std::cos(0.25f * mathConst::pi - 1.1f * arrowDegree);
    float basePointY1 = centerY + ( arcRadius + 0.5f * arrowWidth ) * std::sin(0.25f * mathConst::pi - 1.1f * arrowDegree);
    float basePointX2 = centerX + ( arcRadius - 0.5f * arrowWidth ) * std::cos(0.25f * mathConst::pi - 1.1f * arrowDegree);
    float basePointY2 = centerY + ( arcRadius - 0.5f * arrowWidth ) * std::sin(0.25f * mathConst::pi - 1.1f * arrowDegree);
    juce::Point<float> endPoint(endPointX, endPointY);
    juce::Point<float> basePoint1(basePointX1, basePointY1);
    juce::Point<float> basePoint2(basePointX2, basePointY2);
    arrow.addTriangle(endPoint, basePoint1, basePoint2);
    g.setColour(juce::Colours::black);
    g.fillPath(arrow);
}

void CustomStyle::drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton,
                                   bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    auto bounds = toggleButton.getLocalBounds().toFloat();
    float switchWidth = bounds.getWidth() * 0.2f;
    float switchHeight = bounds.getHeight() * 0.35f;
    float baseDiameter = bounds.getWidth() * 0.7f;
    float baseRadius = baseDiameter / 2.0f;
    float centerX = bounds.getWidth() * 0.5f;
    float centerY = bounds.getHeight() * 0.5f;

    juce::Rectangle<float> baseCircle(centerX - baseRadius, centerY - baseRadius, baseDiameter, baseDiameter); 
    juce::ColourGradient gradOut(juce::Colours::white, centerX, centerY, 
                                 juce::Colours::black.withAlpha(0.4f), centerX + baseRadius, centerY + baseRadius, 
                                 true);
    g.setGradientFill(gradOut);
    g.fillEllipse(baseCircle);
    float internalDiameter = baseDiameter * 0.6f;
    float internalRadius = internalDiameter / 2.0f;
    juce::Rectangle<float> internalCircle(centerX - internalRadius, centerY - internalRadius, internalDiameter, internalDiameter); 
    juce::ColourGradient gradInt(juce::Colours::white, centerX, centerY, 
                                 juce::Colours::black, centerX + internalRadius, centerY + internalRadius, 
                                 true);
    g.setGradientFill(gradInt);
    g.fillEllipse(internalCircle);
    g.setColour(juce::Colours::black);
    g.drawEllipse(internalCircle, 1.0f);
    g.drawEllipse(baseCircle, 1.0f);

    // g.setColour(juce::Colours::black);
    // g.drawRoundedRectangle(bounds, 4.0f, 2.0f);

    float yOffset = toggleButton.getToggleState() ? centerY - switchHeight : centerY;
    auto switchBounds = juce::Rectangle<float>(bounds.getCentreX() - switchWidth / 2, yOffset, switchWidth, switchHeight);
    g.setColour(juce::Colours::silver);
    g.fillRoundedRectangle(switchBounds, 4.0f);
    g.setGradientFill(juce::ColourGradient::vertical(juce::Colours::white, switchBounds.getY(),
                                                     juce::Colours::black.withAlpha(0.6f), switchBounds.getBottom()));
    g.fillRect(switchBounds.reduced(2.0f));
    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(switchBounds, 4.0f, 1.0f);
}

void CustomStyle::drawLabel(juce::Graphics& g, juce::Label& label)
{
    juce::Font comicSansFont("Comic Sans MS", 20.0f, juce::Font::plain);
    g.setFont(comicSansFont);
    g.setColour(juce::Colours::black);
    g.drawFittedText(label.getText(), label.getLocalBounds(), juce::Justification::centred, 1);
}
