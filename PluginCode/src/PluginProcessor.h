#pragma once
#include <JuceHeader.h>
#include "Processing/MonoLimiter.h"
#include "Processing/NeuralNetwork.h"
#include "Processing/ToneControl.h"

//==============================================================================
class OCD_EmuAudioProcessor  : public juce::AudioProcessor 
                               #if JucePlugin_Enable_ARA
                                , public juce::AudioProcessorARAExtension
                               #endif
{
public:
    //==============================================================================
    OCD_EmuAudioProcessor();
    ~OCD_EmuAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    #ifndef JucePlugin_PreferredChannelConfigurations
     bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Public setters for sliders and buttons
    void setDrive(double newValue);
    void setSwitch(bool newState);
    void setTone(double newValue);
    void setVolume(double newValue);
    void setBypass(bool newState); 

private:
    // Instance of a mono limiter to avoid input greater/smaller than 1/-1
    MonoLimiter inputLimiter;

    // Instance of the used neural network
    NeuralNetwork nnModel;

    // Instance of the tone control WDF
    ToneControl toneControl;

    // User parameters to control on the UI
    juce::AudioParameterFloat* driveParam;
    juce::AudioParameterBool* switchParam;
    juce::AudioParameterFloat* toneParam;
    juce::AudioParameterFloat* volumeParam;
    juce::AudioParameterBool* bypassParam;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OCD_EmuAudioProcessor)
};