#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OCD_EmuAudioProcessorEditor::OCD_EmuAudioProcessorEditor(OCD_EmuAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Size of the plugin UI
    setSize(300, 600);

    // Min/Max angle for knob rotation
    float startAngle = -0.75f * mathConst::pi;
    float endAngle = 0.75f * mathConst::pi;

    // Setup of the drive knob
    addAndMakeVisible(driveKnob);
    driveKnob.setRange(0.0, 1.0, 0.01);
    driveKnob.setValue(0.0);
    driveKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    driveKnob.setSliderStyle(juce::Slider::Rotary);
    driveKnob.setRotaryParameters(startAngle, endAngle, true);
    driveKnob.setLookAndFeel(&uiStyle);
    driveKnob.addListener(this);

    // Setup of the switch HP/LP switch
    addAndMakeVisible(switchBtn);
    switchBtn.setLookAndFeel(&uiStyle);
    switchBtn.addListener(this);

    // Setup of the active/bypass LED
    addAndMakeVisible(led);

    // Setup of the tone knob
    addAndMakeVisible(toneKnob);
    toneKnob.setRange(0.0, 1.0, 0.01);
    toneKnob.setValue(0.0);
    toneKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    toneKnob.setSliderStyle(juce::Slider::Rotary);
    toneKnob.setRotaryParameters(startAngle, endAngle, true);
    toneKnob.setLookAndFeel(&uiStyle);
    toneKnob.addListener(this);

    // Setup of the volume knob
    addAndMakeVisible(volumeKnob);
    volumeKnob.setRange(0.0, 1.0, 0.01);
    volumeKnob.setValue(0.5);
    volumeKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    volumeKnob.setSliderStyle(juce::Slider::Rotary);
    volumeKnob.setRotaryParameters(startAngle, endAngle, true);
    volumeKnob.setLookAndFeel(&uiStyle);
    volumeKnob.addListener(this);

    // Label setup for pedal name
    addAndMakeVisible(pedalName);
    pedalName.setFont(juce::Font("Comic Sans MS", float(getHeight()) / 3.5f, juce::Font::bold));
    pedalName.setColour(juce::Label::textColourId, juce::Colours::black);
    pedalName.setJustificationType(juce::Justification::centred);

    // Setup of the bypass switch
    addAndMakeVisible(bypassBtn);
    bypassBtn.setClickingTogglesState(true);
    bypassBtn.setLookAndFeel(&uiStyle);
    bypassBtn.addListener(this);

    // Label setup for each user-controllable parameter
    addAndMakeVisible(driveLbl);
    driveLbl.setLookAndFeel(&uiStyle);
    addAndMakeVisible(highPassLbl);
    highPassLbl.setLookAndFeel(&uiStyle);
    addAndMakeVisible(lowPassLbl);
    lowPassLbl.setLookAndFeel(&uiStyle);
    addAndMakeVisible(toneLbl);
    toneLbl.setLookAndFeel(&uiStyle);
    addAndMakeVisible(volumeLbl);
    volumeLbl.setLookAndFeel(&uiStyle);

    // Personal label setup 
    addAndMakeVisible(manufacturer);
    manufacturer.setFont(juce::Font("Comic Sans MS", 17.0f, juce::Font::plain));
    manufacturer.setColour(juce::Label::textColourId, juce::Colours::black);
    manufacturer.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(project);
    project.setFont(juce::Font("Comic Sans MS", 17.0f, juce::Font::plain));
    project.setColour(juce::Label::textColourId, juce::Colours::black);
    project.setJustificationType(juce::Justification::centred);
}

OCD_EmuAudioProcessorEditor::~OCD_EmuAudioProcessorEditor()
{
    // Deactivate all the custom LookAndFeels
    driveKnob.setLookAndFeel(nullptr);
    switchBtn.setLookAndFeel(nullptr);
    toneKnob.setLookAndFeel(nullptr);
    volumeKnob.setLookAndFeel(nullptr);
    driveLbl.setLookAndFeel(nullptr);
    toneLbl.setLookAndFeel(nullptr);
    volumeLbl.setLookAndFeel(nullptr);
}

//==============================================================================
void OCD_EmuAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background colour
    g.fillAll(juce::Colours::black);

    // Geometric parameters
    float corner = 10.0f;
    float pedalWidth = float(getWidth()) - 2.0f * corner;
    float pedalHeight = float(getHeight()) - 2.0f * corner;

    // Actual pedal background color and geometry
    g.setColour(juce::Colour::fromRGB(253, 253, 208));
    g.fillRoundedRectangle(juce::Rectangle<float>(corner, corner, pedalWidth, pedalHeight), 10);
}

void OCD_EmuAudioProcessorEditor::resized()
{
    // Geometric parameters
    int w = getWidth();
    int h = getHeight(); 

    int knobDiameter = h / 5;
    int upCorner = 30;
    int sxCorner = 15;
    int centerX = w / 2;
    int dxCorner = w - knobDiameter - sxCorner;
    int labelHeight = 15;
 
    // Positions for each control and label
    driveKnob.setBounds(sxCorner, upCorner, knobDiameter, knobDiameter);
    driveLbl.setBounds(sxCorner, upCorner - 12, knobDiameter, labelHeight);

    switchBtn.setBounds(centerX - w / 16, 30, w / 8, h / 10);
    highPassLbl.setBounds(centerX - w / 16, 20, w / 8, 15);
    lowPassLbl.setBounds(centerX - w / 16, 25 + h / 10, w / 8, 15);

    led.setBounds(centerX - 8, 58 + h / 10, 16, 16);

    toneKnob.setBounds(centerX - knobDiameter / 2, knobDiameter + upCorner + 10, knobDiameter, knobDiameter);
    toneLbl.setBounds(centerX - knobDiameter / 2, knobDiameter + upCorner - 2, knobDiameter, labelHeight);

    volumeKnob.setBounds(dxCorner, upCorner, knobDiameter, knobDiameter);
    volumeLbl.setBounds(dxCorner, upCorner - 12, knobDiameter, labelHeight);

    pedalName.setBounds(centerX - w / 2 + 20, h / 2 - 10, w - 40, int(h / 5.5));

    bypassBtn.setBounds(3 * w / 8, 2 * h / 3 + 25, w / 4, w / 4);

    manufacturer.setBounds(centerX - w / 2, h - 75, w, 20);
    project.setBounds(centerX - w / 2, h - 55, w, 20);
}

// Overrides of buttonClicked and silderValueChanged for user interaction
void OCD_EmuAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == &switchBtn)
    {
        audioProcessor.setSwitch(switchBtn.getToggleState());
    }
    if (button == &bypassBtn)
    {
        audioProcessor.setBypass(bypassBtn.getToggleState());
        led.setLedOn(bypassBtn.getToggleState());
    }
}
 
void OCD_EmuAudioProcessorEditor::sliderValueChanged(Slider *slider)
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