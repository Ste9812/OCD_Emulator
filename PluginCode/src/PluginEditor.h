#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

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
    juce::Slider driveRot;
    juce::TextButton switchBtn{"LP"};
    juce::Slider toneRot;
    juce::Slider volumeRot;
    juce::TextButton bypassBtn{"ON"};

    juce::Label driveLbl{{}, "Drive"};
    juce::Label toneLbl{{}, "Tone"};
    juce::Label volumeLbl{{}, "Volume"};

    OCD_EmuAudioProcessor& audioProcessor;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OCD_EmuAudioProcessorEditor)
};
