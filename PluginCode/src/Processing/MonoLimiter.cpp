#include "MonoLimiter.h"

//==============================================================================
MonoLimiter::MonoLimiter()
{
}

//==============================================================================
void MonoLimiter::prepare(double sampleRate, int samplesPerBlock)
{
    limiter.reset();
    limiterSpec.sampleRate = sampleRate;
    limiterSpec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock); 
    limiterSpec.numChannels = static_cast<juce::uint32>(numberChannels);
    limiter.prepare(limiterSpec);
    limiter.setRelease(limiterRelTimeMS);
    limiter.setThreshold(limiterThresholdDB);
}

//==============================================================================
void MonoLimiter::process(juce::AudioBuffer<float>& inputBuffer, float* outputBuffer, int bufferLength)
{
    juce::dsp::AudioBlock<float> audioBlock(inputBuffer);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);
    limiter.process(context);
    float* processedBuffer = context.getOutputBlock().getChannelPointer(0);
    std::copy(processedBuffer, processedBuffer + bufferLength, outputBuffer);
}

