#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OCD_EmuAudioProcessorEditor::OCD_EmuAudioProcessorEditor (OCD_EmuAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(300, 600);

    addAndMakeVisible(driveRot);
    driveRot.setRange(0.0, 1.0, 0.01);
    driveRot.setValue(0.0);
    driveRot.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    driveRot.setSliderStyle(juce::Slider::Rotary);
    driveRot.addListener(this);

    addAndMakeVisible(driveLbl);
    driveLbl.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(switchBtn);
    switchBtn.setClickingTogglesState(true);
    switchBtn.addListener(this);

    addAndMakeVisible(toneRot);
    toneRot.setRange(0.0, 1.0, 0.01);
    toneRot.setValue(0.0);
    toneRot.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    toneRot.setSliderStyle(juce::Slider::Rotary);
    toneRot.addListener(this);

    addAndMakeVisible(toneLbl);
    toneLbl.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(volumeRot);
    volumeRot.setRange(0.0, 1.0, 0.01);
    volumeRot.setValue(0.5);
    volumeRot.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    volumeRot.setSliderStyle(juce::Slider::Rotary);
    volumeRot.addListener(this);

    addAndMakeVisible(volumeLbl);
    volumeLbl.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(bypassBtn);
    bypassBtn.setClickingTogglesState(true);
    bypassBtn.addListener(this);
}

OCD_EmuAudioProcessorEditor::~OCD_EmuAudioProcessorEditor()
{
}

//==============================================================================
void OCD_EmuAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
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
 
    driveRot.setBounds(sxCorner, upCorner, rotDim, rotDim);
    driveLbl.setBounds(sxCorner, upCorner - 15, rotDim, 15);

    switchBtn.setBounds(center - w / 16, upCorner / 2, w / 8, h / 10);

    toneRot.setBounds(center - rotDim / 2, rotDim + upCorner + 10, rotDim, rotDim);
    toneLbl.setBounds(center - rotDim / 2, rotDim + upCorner - 5, rotDim, 15);

    volumeRot.setBounds(dxCorner, upCorner, rotDim, rotDim);
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
    if (slider == &driveRot)
    {
        audioProcessor.setDrive(slider->getValue());
    }
    if (slider == &toneRot)
    {
        audioProcessor.setTone(slider->getValue());
    }
    if (slider == &volumeRot)
    {
        audioProcessor.setVolume(slider->getValue());
    }
}