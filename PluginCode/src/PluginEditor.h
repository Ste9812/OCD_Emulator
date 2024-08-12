#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UserInterface/CustomStyle.h"

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
    juce::TextButton switchBtn{"LP"};
    juce::Slider toneKnob;
    juce::Slider volumeKnob;
    juce::TextButton bypassBtn{"ON"};

    juce::Label driveLbl{{}, "Drive"};
    juce::Label toneLbl{{}, "Tone"};
    juce::Label volumeLbl{{}, "Volume"};

    CustomStyle uiStyle;

    OCD_EmuAudioProcessor& audioProcessor;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OCD_EmuAudioProcessorEditor)
};
