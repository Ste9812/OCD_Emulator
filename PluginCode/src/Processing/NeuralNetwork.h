#pragma once
#include "RTNeural/RTNeural.h"

using floatVec = std::vector<float>;
using floatVec2D = std::vector<floatVec>;
using denseLayer = RTNeural::DenseT<float, 32, 1>;
using lstmLayer = RTNeural::LSTMLayerT<float, 2, 32, RTNeural::SampleRateCorrectionMode::LinInterp>;
using conditionedLSTM = RTNeural::ModelT<float, 2, 1, lstmLayer, denseLayer>;

class NeuralNetwork 
{
public:
    NeuralNetwork() = default;
    NeuralNetwork(const char* filename);

    void prepare(double sampleRate);

    void process(float* buffer, int bufferLength);

    void setDrive(float value);

    floatVec NeuralNetwork::sum(const floatVec& x, const floatVec& y);
    floatVec2D transpose(const floatVec2D& x);

private:
    conditionedLSTM model;

    // Default sample rate for the neural network (training sample rate)
    double defaultSampleRate = 48000.0;

    // Sample rate multiplication factor
    float sampleRateMult;

    // Drive parameter
    float driveGain;

    // Auxiliary vector for feeding the neural network
    float parInput[2] = {0.0f, 0.0f};

    void loadWeights(const char* filename);
};