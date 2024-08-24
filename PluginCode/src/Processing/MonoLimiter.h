#pragma once
#include <JuceHeader.h>
#include <modules/juce_dsp/juce_dsp.h>

class MonoLimiter
{
public:
    // Default constructor
    MonoLimiter();
 
    // Processor preparing function
    void prepare(double sampleRate, int samplesPerBlock);

    // Main audio processing function
    void process(juce::AudioBuffer<float>& inputBuffer, float* outputBuffer, int bufferLength);

private:
    // Instance of specifications for a JUCE processor
    juce::dsp::ProcessSpec limiterSpec;

    // Number of channels (mono)
    int numberChannels = 1;

    // Instance of the basic limiter provided by JUCE
    juce::dsp::Limiter<float> limiter;

    // Fixed values for the limiter
    float limiterRelTimeMS = 10.0f;
    float limiterThresholdDB = 0.0f;
};