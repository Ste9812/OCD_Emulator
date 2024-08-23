#pragma once
#include "RTNeural/RTNeural.h"
#include <JuceHeader.h>

// Specify the hidden size of the recurrent layer
const int hiddenSize = 32; 

// Identifier for vectors and nested vectors
using floatVec = std::vector<float>;
using floatVec2D = std::vector<floatVec>;

// Identifier for recurrent and dense layers
using denseLayer = RTNeural::DenseT<float, hiddenSize, 1>;
using lstmLayer = RTNeural::LSTMLayerT<float, 2, hiddenSize, RTNeural::SampleRateCorrectionMode::LinInterp>;

// Identifier for the complete model structure
using condModel = RTNeural::ModelT<float, 2, 1, lstmLayer, denseLayer>;   

class NeuralNetwork 
{
public:
    // Default constructor
    NeuralNetwork();

    // Function to adapt the network to the current sampling frequency
    void prepare(double sampleRate);

    // Main signal processing function of the network
    void process(float* buffer, int bufferLength);

    // Function to update the drive parameter
    void setDrive(float value);

private:
    // Instance of the conditioned network model
    condModel model;

    // Default sample rate for the neural network (training sample rate)
    double defaultSampleRate = 48000.0;

    // Sample rate multiplication factor
    float sampleRateMult;

    // Drive parameter
    float driveGain;

    // Auxiliary vector for feeding the neural network
    float parInput[2] = {0.0f, 0.0f};

    // Private method for loading weights and biases
    void loadWeights();

    // Auxiliary functions for vector and nested vectors operations
    floatVec NeuralNetwork::sum(const floatVec& x, const floatVec& y);
    floatVec2D transpose(const floatVec2D& x);
};