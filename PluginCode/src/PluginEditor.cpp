#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OCD_EmuAudioProcessorEditor::OCD_EmuAudioProcessorEditor (OCD_EmuAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(300, 600);

    float startAngle = -0.75f * juce::MathConstants<float>::pi;
    float endAngle = 0.75f * juce::MathConstants<float>::pi;

    addAndMakeVisible(driveKnob);
    driveKnob.setRange(0.0, 1.0, 0.01);
    driveKnob.setValue(0.0);
    driveKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    driveKnob.setSliderStyle(juce::Slider::Rotary);
    driveKnob.setRotaryParameters(startAngle, endAngle, true);
    driveKnob.setLookAndFeel(&uiStyle);
    driveKnob.addListener(this);

    addAndMakeVisible(switchBtn);
    switchBtn.setClickingTogglesState(true);
    switchBtn.addListener(this);

    addAndMakeVisible(toneKnob);
    toneKnob.setRange(0.0, 1.0, 0.01);
    toneKnob.setValue(0.0);
    toneKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    toneKnob.setSliderStyle(juce::Slider::Rotary);
    toneKnob.setRotaryParameters(startAngle, endAngle, true);
    toneKnob.setLookAndFeel(&uiStyle);
    toneKnob.addListener(this);

    addAndMakeVisible(volumeKnob);
    volumeKnob.setRange(0.0, 1.0, 0.01);
    volumeKnob.setValue(0.5);
    volumeKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    volumeKnob.setSliderStyle(juce::Slider::Rotary);
    volumeKnob.setRotaryParameters(startAngle, endAngle, true);
    volumeKnob.setLookAndFeel(&uiStyle);
    volumeKnob.addListener(this);

    addAndMakeVisible(bypassBtn);
    bypassBtn.setClickingTogglesState(true);
    bypassBtn.addListener(this);

    addAndMakeVisible(driveLbl);
    driveLbl.setLookAndFeel(&uiStyle);

    addAndMakeVisible(toneLbl);
    toneLbl.setLookAndFeel(&uiStyle);

    addAndMakeVisible(volumeLbl);
    volumeLbl.setLookAndFeel(&uiStyle);
}

OCD_EmuAudioProcessorEditor::~OCD_EmuAudioProcessorEditor()
{
    driveKnob.setLookAndFeel(nullptr);
    toneKnob.setLookAndFeel(nullptr);
    volumeKnob.setLookAndFeel(nullptr);
    driveLbl.setLookAndFeel(nullptr);
    toneLbl.setLookAndFeel(nullptr);
    volumeLbl.setLookAndFeel(nullptr);
}

//==============================================================================
void OCD_EmuAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    float corner = 10.0f;
    float pedalWidth = float(getWidth()) - 2.0f * corner;
    float pedalHeight = float(getHeight()) - 2.0f * corner;

    g.setColour(juce::Colour::fromRGB(253, 253, 208));
    g.fillRoundedRectangle(juce::Rectangle<float>(corner, corner, pedalWidth, pedalHeight), 10);
}

void OCD_EmuAudioProcessorEditor::resized()
{
    int w = getWidth();
    int h = getHeight(); 

    int knobDiameter = h / 5;
    int upCorner = 30;
    int sxCorner = 10;
    int centerX = w / 2;
    int dxCorner = w - knobDiameter - sxCorner;
    int labelHeight = 15;
 
    driveKnob.setBounds(sxCorner, upCorner, knobDiameter, knobDiameter);
    driveLbl.setBounds(sxCorner, upCorner - 10, knobDiameter, labelHeight);

    switchBtn.setBounds(centerX - w / 16, upCorner / 2, w / 8, h / 10);

    toneKnob.setBounds(centerX - knobDiameter / 2, knobDiameter + upCorner + 10, knobDiameter, knobDiameter);
    toneLbl.setBounds(centerX - knobDiameter / 2, knobDiameter + upCorner, knobDiameter, labelHeight);

    volumeKnob.setBounds(dxCorner, upCorner, knobDiameter, knobDiameter);
    volumeLbl.setBounds(dxCorner, upCorner - 10, knobDiameter, labelHeight);

    bypassBtn.setBounds(w / 2 - w / 7, 2 * h / 3, 2 * w / 7, 2 * w / 7);
}

void OCD_EmuAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == &switchBtn)
    {
        audioProcessor.setSwitch(switchBtn.getToggleState());
        if (!switchBtn.getToggleState())
        {
            switchBtn.setButtonText("HP");
        }
        else
        {
            switchBtn.setButtonText("LP");
        }
    }
    if (button == &bypassBtn)
    {
        audioProcessor.setEnable(bypassBtn.getToggleState());
        if (!bypassBtn.getToggleState())
        {
            bypassBtn.setButtonText("OFF");
        }
        else
        {
            bypassBtn.setButtonText("ON");
        }
    }
}
 
void OCD_EmuAudioProcessorEditor::sliderValueChanged (Slider *slider)
{
    if (slider == &driveKnob)
    {
        audioProcessor.setDrive(slider->getValue());
    }
    if (slider == &toneKnob)
    {
        audioProcessor.setTone(slider->getValue());
    }
    if (slider == &volumeKnob)
    {
        audioProcessor.setVolume(slider->getValue());
    }
}