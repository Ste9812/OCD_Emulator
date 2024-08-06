#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OCD_EmuAudioProcessor::OCD_EmuAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput("Output", juce::AudioChannelSet::mono(), true)
                     #endif
                       )
#endif

{
    //setupModel(lstmModel, "model.json");

    B_V << -1, 0,  0,  0, 0,  0, 0, 1, 0, 0, 0, 
            0, 0,  0, -1, 1,  0, 0, 0, 1, 0, 0, 
            1, 1, -1,  1, 0, -1, 0, 0, 0, 1, 0, 
            0, 0, -1,  0, 0, -1, 1, 0, 0, 0, 1;

    B_I << -1, 0,  0, 0, 0,  0, 0, 1, 0, 0, 0,
            1, 0,  0, 0, 1,  0, 0, 0, 1, 0, 0,
            0, 1, -1, 0, 0, -1, 0, 0, 0, 1, 0,
            0, 0, -1, 0, 0, -1, 1, 0, 0, 0, 1;

    Rsw = Rs1;
    Rtc = Rtol;
    Rva = 0.1 * Rv + Rtol;
    Rvb = 0.9 * Rv + Rtol;

    I = Eigen::MatrixXd::Identity(11, 11);

    driveParam = new juce::AudioParameterFloat("drivePar", "Drive", 0.0f, 1.0f, 0.0f);
    addParameter(driveParam);
    switchParam = new juce::AudioParameterBool("switchPar", "Switch", true);
    addParameter(switchParam);
    toneParam = new juce::AudioParameterFloat("tonePar", "Tone", 0.0f, 1.0f, 0.0f);
    addParameter(toneParam); 
    volumeParam = new juce::AudioParameterFloat("volumePar", "Volume", 0.0f, 1.0f, 0.5f); 
    addParameter(volumeParam);
    enableParam = new juce::AudioParameterBool("enablePar", "Enable", true);  
    addParameter(enableParam);
}

OCD_EmuAudioProcessor::~OCD_EmuAudioProcessor()
{
}

//==============================================================================
const juce::String OCD_EmuAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OCD_EmuAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OCD_EmuAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OCD_EmuAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OCD_EmuAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OCD_EmuAudioProcessor::getNumPrograms()
{
    return 1;
}

int OCD_EmuAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OCD_EmuAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String OCD_EmuAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void OCD_EmuAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void OCD_EmuAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);

    double Z_g = Rg;
    double Z_R9 = R9;
    double Z_R10 = R10;
    double Z_Rsw = Rsw;
    double Z_Rtc = Rtc;
    double Z_Rva = Rva;
    double Z_Rvb = Rvb;
    double Z_C5 = 1 / (2 * C5 * sampleRate);
    double Z_C6 = 1 / (2 * C6 * sampleRate);
    double Z_C7 = 1 / (2 * C7 * sampleRate);
    double Z_C8 = 1 / (2 * C8 * sampleRate);

    Z_diag << Z_C5 , Z_C7 , Z_C8 , Z_g , Z_C6 , Z_Rtc , Z_Rva , Z_R9 , Z_R10 , Z_Rsw , Z_Rvb;

    a = Eigen::MatrixXd::Zero(11, 1);
    b = Eigen::MatrixXd::Zero(11, 1);
}

void OCD_EmuAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OCD_EmuAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OCD_EmuAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* outData = buffer.getWritePointer(channel);
        auto* inData = buffer.getReadPointer(channel);
            for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                processWDF(&inData[sample]);
                outData[sample] = static_cast<float>((a(6, 0) + b(6, 0)) / 2.0);
            }
    }
}

//==============================================================================
bool OCD_EmuAudioProcessor::hasEditor() const
{
    return true; 
}

juce::AudioProcessorEditor* OCD_EmuAudioProcessor::createEditor()
{
    return new OCD_EmuAudioProcessorEditor (*this);
}

//==============================================================================
void OCD_EmuAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ignoreUnused(destData);
}

void OCD_EmuAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OCD_EmuAudioProcessor();
}

/*
void OCD_EmuAudioProcessor::setupModel(RTLSTMModel32& model, std::string jsonFile)
{
    int lstm_units = 32;
    std::ifstream jsonStream(jsonFile, std::ifstream::binary);
    nlohmann::json modelJson;
    jsonStream >> modelJson;
    auto& lstm = model.get<0>();
    const int json_lstm_size = int(modelJson["lstm.weight_ih_l0"].size()) / 4;
    if (json_lstm_size != lstm_units)
    {
        std::cout << "Model wants " << lstm_units << " lstm units but JSON file specifies " << json_lstm_size << std::endl;
        throw(std::exception());
    }
    RTNeural::torch_helpers::loadLSTM<float> (modelJson, "lstm.", lstm);
    auto& dense = model.get<1>();
    RTNeural::torch_helpers::loadDense<float> (modelJson, "dense.", dense);
}
*/

void OCD_EmuAudioProcessor::setDrive(double newValue)
{
    *driveParam = (float)newValue;
}

void OCD_EmuAudioProcessor::setSwitch(bool newState)
{
    *switchParam = newState;
    if (*switchParam)
    {
        Rsw = Rs1;
    }
    else
    {
        Rsw = Rs2;
    }
    Z_diag(0, 9) = Rsw;
    updateS();
}

void OCD_EmuAudioProcessor::setTone(double newValue)
{
    *toneParam = (float)newValue;
    Rtc = Rt * static_cast<double>(*toneParam) + Rtol;
    Z_diag(0, 5) = Rtc;
    updateS();
}

void OCD_EmuAudioProcessor::setVolume(double newValue)
{
    *volumeParam = (float)newValue;
    Rva = Rv * (A * pow(B, static_cast<double>(*volumeParam)) + C) + Rtol;
    Rvb = Rv * (1 - A * pow(B, static_cast<double>(*volumeParam)) - C) + Rtol;
    Z_diag(0, 6) = Rva;
    Z_diag(0, 10) = Rvb;
    updateS();
}

void OCD_EmuAudioProcessor::setEnable(bool newState)
{
    *enableParam = newState;
}

void OCD_EmuAudioProcessor::updateS()
{
    Z = Z_diag.asDiagonal();
    S = I - 2 * Z * B_I.transpose() * (B_V * Z * B_I.transpose()).inverse() * B_V;
}

void OCD_EmuAudioProcessor::processWDF(const float *in)
{
    b(0, 0) = a(0, 0);
    b(1, 0) = a(1, 0);
    b(2, 0) = a(2, 0);
    b(3, 0) = double(*in);
    b(4, 0) = a(4, 0);
    b(5, 0) = 0.0;
    b(6, 0) = 0.0;
    b(7, 0) = 0.0;
    b(8, 0) = 0.0;
    b(9, 0) = 0.0;
    b(10, 0) = 0.0;

    a = S * b;
}