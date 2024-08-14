#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UserInterface/CustomStyle.h"
#include "UserInterface/Led.h"

//==============================================================================
class OCD_EmuAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public Slider::Listener, 
                                        public Button::Listener
{
public:
    OCD_EmuAudioProcessorEditor (OCD_EmuAudioProcessor&);
    ~OCD_EmuAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void sliderValueChanged(Slider *slider) override;
    void buttonClicked(Button* btn) override; 

private:
    juce::Slider driveKnob;
    juce::ToggleButton switchBtn;
    juce::Slider toneKnob;
    juce::Slider volumeKnob;
    juce::TextButton bypassBtn;

    juce::Label driveLbl{{}, "Drive"};
    juce::Label highPassLbl{{}, "HP"};
    juce::Label lowPassLbl{{}, "LP"};
    juce::Label toneLbl{{}, "Tone"};
    juce::Label volumeLbl{{}, "Volume"};

    juce::Label pedalName{{}, "OCD"};
    juce::Label manufacturer{{}, "Stefano Ravasi"};
    juce::Label project{{}, "STMAE Project - 2023/2024"};

    Led led;

    CustomStyle uiStyle;

    OCD_EmuAudioProcessor& audioProcessor;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OCD_EmuAudioProcessorEditor)
};
