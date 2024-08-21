#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OCD_EmuAudioProcessor::OCD_EmuAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                      #if ! JucePlugin_IsMidiEffect
                       #if ! JucePlugin_IsSynth
                        .withInput("Input",  AudioChannelSet::mono(), true)
                       #endif
                        .withOutput("Output", AudioChannelSet::mono(), true)
                      #endif
                       )
#endif
{
    // Path of the neural network model
    const char *modelPath = "C:/Users/Stefano/Documents/GitHub/STMAE_Project/Training/Results/OCD_Parameterized-LSTM-OCD_Parameterized/model.json"; 

    // Build of the neural network according to the model embedded in the .json file
    nnModel = NeuralNetwork::NeuralNetwork(modelPath);

    // Setting up each parameter with default values and IDs
    driveParam = new juce::AudioParameterFloat("drivePar", "Drive", 0.0f, 1.0f, 0.0f);
    addParameter(driveParam);
    switchParam = new juce::AudioParameterBool("switchPar", "Switch", false);
    addParameter(switchParam);
    toneParam = new juce::AudioParameterFloat("tonePar", "Tone", 0.0f, 1.0f, 0.0f);
    addParameter(toneParam); 
    volumeParam = new juce::AudioParameterFloat("volumePar", "Volume", 0.0f, 1.0f, 0.5f); 
    addParameter(volumeParam);
    bypassParam = new juce::AudioParameterBool("enablePar", "Enable", false);  
    addParameter(bypassParam);
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
    // Each signal processor is prepared each time the sample rate or the block size change
    inputLimiter.prepare(sampleRate, samplesPerBlock);

    nnModel.prepare(sampleRate);

    toneControl.prepare(sampleRate);
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

void OCD_EmuAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    auto bufferLength = buffer.getNumSamples();

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, bufferLength);
    }

    auto* outputBuffer = buffer.getWritePointer(0);

    // If the bypass is on, no processing is applied to the audio block
    if (*bypassParam)
    {
        juce::dsp::AudioBlock<float> audioBlock(buffer);
        float* unprocessedBuffer = audioBlock.getChannelPointer(0);
        std::copy(unprocessedBuffer, unprocessedBuffer + bufferLength, outputBuffer);
    }
    else
    {
        inputLimiter.process(buffer, outputBuffer, bufferLength);
        nnModel.process(outputBuffer, bufferLength);
        toneControl.process(outputBuffer, bufferLength);
    }

    // cautional passage to account for eventual extra output channel requested by the host
    for (int ch = 1; ch < totalNumInputChannels; ++ch)
    {
        auto* outData = buffer.getWritePointer(ch);
        std::copy(outputBuffer, outputBuffer + bufferLength, outData);
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

// The following functions are setters for sliders and buttons on the UI
void OCD_EmuAudioProcessor::setDrive(double newValue)
{
    *driveParam = (float)newValue;
    nnModel.setDrive(static_cast<float>(*driveParam));
}

void OCD_EmuAudioProcessor::setSwitch(bool newState)
{
    *switchParam = newState;
    toneControl.setSwitch(static_cast<bool>(*switchParam));
}

void OCD_EmuAudioProcessor::setTone(double newValue)
{
    *toneParam = (float)newValue;
    toneControl.setTone(static_cast<double>(*toneParam));
}

void OCD_EmuAudioProcessor::setVolume(double newValue)
{
    *volumeParam = (float)newValue;
    toneControl.setVolume(static_cast<double>(*volumeParam));
}

void OCD_EmuAudioProcessor::setBypass(bool newState)
{
    *bypassParam = newState;
}