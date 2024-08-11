#pragma once
#include <JuceHeader.h>
#include <modules/juce_dsp/juce_dsp.h>

class MonoLimiter
{
public:
    MonoLimiter();

    void prepare(double sampleRate, int samplesPerBlock);

    void process(juce::AudioBuffer<float>& inputBuffer, float* outputBuffer, int bufferLength);

private:
    juce::dsp::ProcessSpec limiterSpec;

    int numberChannels = 1;

    juce::dsp::Limiter<float> limiter;

    float limiterRelTimeMS = 10.0f;
    float limiterThresholdDB = 0.0f;
};