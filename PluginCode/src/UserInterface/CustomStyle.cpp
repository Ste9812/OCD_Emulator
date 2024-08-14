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
    float centreX = float(x) + float(width) / 2.0f;
    float centreY = float(y) + float(height) / 2.0f;
    float axisX = centreX - radius;
    float axisY = centreY - radius;
    float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    float tickWidth = 5.0f;

    auto black = juce::Colours::black;
    auto white = juce::Colours::whitesmoke;

    juce::Rectangle<float> knobArea(axisX, axisY, diameter, diameter);
    g.setColour(black);
    g.fillEllipse(knobArea);

    juce::Path knobTick;
    knobTick.addRectangle(-0.5f * tickWidth, -0.8f * radius, tickWidth, 0.6f * radius);
    g.setColour(white);
    g.fillPath(knobTick, juce::AffineTransform::rotation(angle).translated(centreX, centreY));   

    juce::Path arcPath;
    float arcRadius = radius + 6.0f; 
    float arrowDegree = 10.0f / 180.0f * mathConst::pi;
    arcPath.addArc(centreX - arcRadius, centreY - arcRadius, 2.0f * arcRadius, 2.0f * arcRadius, rotaryStartAngle, rotaryEndAngle - arrowDegree, true);
    g.setColour(black);
    g.strokePath(arcPath, juce::PathStrokeType(3.0f));

    juce::Path arrow;
    float arrowWidth = 8.0f;
    float endPointX = centreX + arcRadius * std::cos(0.25f * mathConst::pi + 0.45f * arrowDegree);
    float endPointY = centreY + arcRadius * std::sin(0.25f * mathConst::pi + 0.45f * arrowDegree);
    float basePointX1 = centreX + ( arcRadius + 0.5f * arrowWidth ) * std::cos(0.25f * mathConst::pi - 1.1f * arrowDegree);
    float basePointY1 = centreY + ( arcRadius + 0.5f * arrowWidth ) * std::sin(0.25f * mathConst::pi - 1.1f * arrowDegree);
    float basePointX2 = centreX + ( arcRadius - 0.5f * arrowWidth ) * std::cos(0.25f * mathConst::pi - 1.1f * arrowDegree);
    float basePointY2 = centreY + ( arcRadius - 0.5f * arrowWidth ) * std::sin(0.25f * mathConst::pi - 1.1f * arrowDegree);
    juce::Point<float> endPoint(endPointX, endPointY);
    juce::Point<float> basePoint1(basePointX1, basePointY1);
    juce::Point<float> basePoint2(basePointX2, basePointY2);
    arrow.addTriangle(endPoint, basePoint1, basePoint2);
    g.setColour(black);
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
    float centreX = bounds.getWidth() * 0.5f;
    float centreY = bounds.getHeight() * 0.5f;

    auto black = juce::Colours::black;
    auto white = juce::Colours::white;
    auto silver = juce::Colours::silver;

    juce::Rectangle<float> baseCircle(centreX - baseRadius, centreY - baseRadius, baseDiameter, baseDiameter); 
    colGrad gradOut(white, centreX, centreY, black.withAlpha(0.4f), centreX + baseRadius, centreY + baseRadius, true);
    g.setGradientFill(gradOut);
    g.fillEllipse(baseCircle);
    g.setColour(black);
    g.drawEllipse(baseCircle, 1.0f);

    float intDiameter = baseDiameter * 0.6f;
    float intRadius = intDiameter / 2.0f;
    juce::Rectangle<float> internalCircle(centreX - intRadius, centreY - intRadius, intDiameter, intDiameter); 
    colGrad gradInt(white, centreX, centreY, black, centreX + intRadius, centreY + intRadius, true);
    g.setGradientFill(gradInt);
    g.fillEllipse(internalCircle);
    g.setColour(black);
    g.drawEllipse(internalCircle, 1.0f);

    float yOffset = toggleButton.getToggleState() ? centreY - switchHeight : centreY;
    auto switchBounds = juce::Rectangle<float>(bounds.getCentreX() - switchWidth / 2, yOffset, switchWidth, switchHeight);
    g.setColour(silver);
    g.fillRoundedRectangle(switchBounds, 4.0f);
    g.setGradientFill(colGrad::vertical(white, switchBounds.getY(), black.withAlpha(0.6f), switchBounds.getBottom()));
    g.fillRect(switchBounds.reduced(2.0f));
    g.setColour(black);
    g.drawRoundedRectangle(switchBounds, 4.0f, 1.0f);
}

void CustomStyle::drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
                                       bool isMouseOverButton, bool isButtonDown)
{
    juce::ignoreUnused(backgroundColour, isMouseOverButton);
    
    auto bounds = button.getLocalBounds().toFloat();
    auto centre = bounds.getCentre();
    float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 10.0f;

    auto white = juce::Colours::white;
    auto black = juce::Colours::black;
    auto silver = juce::Colours::silver;
    auto darkgrey = juce::Colours::darkgrey;

    float outRadius = radius * 1.3f;
    float outDiameter = 2.0f * outRadius;
    colGrad gradOut(white, centre.x, centre.y, black.withAlpha(0.4f), centre.x + outRadius, centre.y + outRadius, true);
    g.setGradientFill(gradOut);
    g.fillEllipse(centre.x - outRadius, centre.y - outRadius, outDiameter, outDiameter);
    g.setColour(black);
    g.drawEllipse(centre.x - outRadius, centre.y - outRadius, outDiameter, outDiameter, 0.6f);

    Path hexagon;
    float hexRadius = radius * 1.2f;
    hexagon.addPolygon(centre, 6, hexRadius, mathConst::pi / 6.0f);
    colGrad hexGrad(white, centre.x, centre.y, black.brighter(0.4f), centre.x + hexRadius, centre.y + hexRadius, true);
    g.setGradientFill(hexGrad);
    g.fillPath(hexagon);
    g.setColour(black);
    g.strokePath(hexagon, PathStrokeType(0.6f));

    float intRadius = 0.9f * radius;
    float intDiameter = 2.0f * intRadius;
    float offset = intRadius / float(sqrt(2));
    colGrad grad(silver.brighter(1.2f), centre.x - offset, centre.y - offset, silver.darker(1.0f), centre.x + offset, centre.y + offset, false);
    g.setGradientFill(grad);
    g.fillEllipse(centre.x - intRadius, centre.y - intRadius, intDiameter, intDiameter);
    g.setColour(black);
    g.drawEllipse(centre.x - intRadius, centre.y - intRadius, intDiameter, intDiameter, 0.6f);

    if (isButtonDown)
    {
        g.setColour(darkgrey.brighter(0.4f));
        g.fillEllipse(centre.x - intRadius, centre.y - intRadius, intDiameter, intDiameter);
        g.setColour(black);
        g.drawEllipse(centre.x - intRadius, centre.y - intRadius, intDiameter, intDiameter, 0.6f);
    } 
}

void CustomStyle::drawLabel(juce::Graphics& g, juce::Label& label)
{
    juce::Font comicSansFont("Comic Sans MS", 20.0f, juce::Font::plain);
    g.setFont(comicSansFont);
    g.setColour(juce::Colours::black);
    g.drawFittedText(label.getText(), label.getLocalBounds(), juce::Justification::centred, 1);
}
