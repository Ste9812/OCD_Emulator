#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OCD_EmuAudioProcessorEditor::OCD_EmuAudioProcessorEditor (OCD_EmuAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(300, 600);

    addAndMakeVisible(driveKnob);
    driveKnob.setRange(0.0, 1.0, 0.01);
    driveKnob.setValue(0.0);
    driveKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    driveKnob.setSliderStyle(juce::Slider::Rotary);
    driveKnob.setLookAndFeel(&knobStyle);
    driveKnob.addListener(this);

    addAndMakeVisible(switchBtn);
    switchBtn.setClickingTogglesState(true);
    switchBtn.addListener(this);

    addAndMakeVisible(toneKnob);
    toneKnob.setRange(0.0, 1.0, 0.01);
    toneKnob.setValue(0.0);
    toneKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    toneKnob.setSliderStyle(juce::Slider::Rotary);
    toneKnob.setLookAndFeel(&knobStyle);
    toneKnob.addListener(this);

    addAndMakeVisible(volumeKnob);
    volumeKnob.setRange(0.0, 1.0, 0.01);
    volumeKnob.setValue(0.5);
    volumeKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    volumeKnob.setSliderStyle(juce::Slider::Rotary);
    volumeKnob.setLookAndFeel(&knobStyle);
    volumeKnob.addListener(this);

    addAndMakeVisible(bypassBtn);
    bypassBtn.setClickingTogglesState(true);
    bypassBtn.addListener(this);

    addAndMakeVisible(driveLbl);
    driveLbl.setJustificationType(juce::Justification::centred);
    driveLbl.setColour(1, juce::Colours::black);

    addAndMakeVisible(toneLbl);
    toneLbl.setJustificationType(juce::Justification::centred);
    toneLbl.setColour(1, juce::Colours::black);

    addAndMakeVisible(volumeLbl);
    volumeLbl.setJustificationType(juce::Justification::centred);
    volumeLbl.setColour(1, juce::Colours::black);
}

OCD_EmuAudioProcessorEditor::~OCD_EmuAudioProcessorEditor()
{
    driveKnob.setLookAndFeel(nullptr);
    toneKnob.setLookAndFeel(nullptr);
    volumeKnob.setLookAndFeel(nullptr);
}

//==============================================================================
void OCD_EmuAudioProcessorEditor::paint (juce::Graphics& g)
{
    int corner = 10;
    int pedalWidth = getWidth() - 2 * corner;
    int pedalHeight = getHeight() - 2 * corner;
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colour::fromRGB(253, 253, 208));
    g.fillRect (corner, corner, pedalWidth, pedalHeight);
}

void OCD_EmuAudioProcessorEditor::resized()
{
    int w = getWidth();
    int h = getHeight(); 

    int rotDim = h / 5;
    int upCorner = 30;
    int sxCorner = 10;
    int center = w / 2;
    int dxCorner = w - rotDim - sxCorner;
 
    driveKnob.setBounds(sxCorner, upCorner, rotDim, rotDim);
    driveLbl.setBounds(sxCorner, upCorner - 15, rotDim, 15);

    switchBtn.setBounds(center - w / 16, upCorner / 2, w / 8, h / 10);

    toneKnob.setBounds(center - rotDim / 2, rotDim + upCorner + 10, rotDim, rotDim);
    toneLbl.setBounds(center - rotDim / 2, rotDim + upCorner - 5, rotDim, 15);

    volumeKnob.setBounds(dxCorner, upCorner, rotDim, rotDim);
    volumeLbl.setBounds(dxCorner, upCorner - 15, rotDim, 15);

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