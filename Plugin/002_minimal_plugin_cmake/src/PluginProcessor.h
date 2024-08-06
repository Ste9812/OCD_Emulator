#pragma once
#include <JuceHeader.h>
#include <Eigen/Dense>
#include <iostream>

//#include <RTNeural/RTNeural.h>

//using RTLSTMModel32 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 32>, RTNeural::DenseT<float, 32, 1>>;

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

    //==============================================================================
    void setDrive(double newValue);
    void setSwitch(bool newState);
    void setTone(double newValue);
    void setVolume(double newValue);
    void setEnable(bool newState);

private:
    //RTLSTMModel32 lstmModel;

    //void setupModel(RTLSTMModel32& model, std::string jsonFile);

    juce::AudioParameterFloat* driveParam;
    juce::AudioParameterBool* switchParam;
    juce::AudioParameterFloat* toneParam;
    juce::AudioParameterFloat* volumeParam;
    juce::AudioParameterBool* enableParam;

    double A = 125e-4;
    double B = 81;
    double C = -125e-4;
    double Rg = 1e-9;
    double R9 = 150e3;
    double R10 = 39e3;
    double C5 = 220e-12;
    double C6 = 100e-9;
    double C7 = 1e-6;
    double C8 = 47e-9;
    double Rs1 = 33e3;
    double Rs2 = 13.2e3;
    double Rt = 10e3;
    double Rv = 500e3;
    double Rtol = 1e-6;
    double Rsw;
    double Rtc;
    double Rva;
    double Rvb;

    Eigen::Matrix<double, 4, 11> B_V;
    Eigen::Matrix<double, 4, 11> B_I;
    Eigen::Matrix<double, 11, 11> Z;
    Eigen::Matrix<double, 11, 11> S;
    Eigen::Matrix<double, 11, 11> I;
    Eigen::Matrix<double, 1, 11> Z_diag;
    Eigen::Matrix<double, 11, 1> a;
    Eigen::Matrix<double, 11, 1> b;

    void updateS();
    void processWDF(const float *in);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OCD_EmuAudioProcessor)
};
