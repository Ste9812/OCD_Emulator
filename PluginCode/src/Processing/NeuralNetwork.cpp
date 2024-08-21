#include "NeuralNetwork.h"

// Custom constructor of the neural network class
NeuralNetwork::NeuralNetwork(const char* filePath)
{
    this->loadWeights(filePath);
}

// This function loads the weights and the biases for both the recurrent and the dense layer from a .json file
void NeuralNetwork::loadWeights(const char* filePath)
    {
        std::ifstream jsonStream(filePath);
        nlohmann::json weightsJson;
        jsonStream >> weightsJson;
        auto& recLayer = model.template get<0>();
        auto& dense = model.template get<1>();
        floatVec2D weights_ih = weightsJson["/state_dict/rec.weight_ih_l0"_json_pointer];
        recLayer.setWVals(transpose(weights_ih));
        floatVec2D weights_hh = weightsJson["/state_dict/rec.weight_hh_l0"_json_pointer];
        recLayer.setUVals(transpose(weights_hh));
        floatVec bias_ih = weightsJson["/state_dict/rec.bias_ih_l0"_json_pointer];
        floatVec bias_hh = weightsJson["/state_dict/rec.bias_hh_l0"_json_pointer];
        recLayer.setBVals(sum(bias_ih, bias_hh));
        floatVec2D lin_weights = weightsJson["/state_dict/lin.weight"_json_pointer];
        dense.setWeights(lin_weights);
        floatVec lin_bias = weightsJson["/state_dict/lin.bias"_json_pointer];
        dense.setBias(lin_bias.data());
        model.reset();  
    }

// This function adapts the model to the current sampling frequency if it is higher than the training one
void NeuralNetwork::prepare(double sampleRate) 
{
    if (sampleRate <= defaultSampleRate)
    {
        sampleRateMult = 1.0f;
    }
    else
    {
        sampleRateMult = (float)(sampleRate / defaultSampleRate);
    }
    model.template get<0>().prepare(sampleRateMult);
    model.reset();
}

// This function processes an entire buffer of sample, creating distortion
void NeuralNetwork::process(float* buffer, int bufferLength)
{
    for (int sample = 0; sample < bufferLength; ++sample)
    {
        parInput[0] = buffer[sample];
        parInput[1] = driveGain;
        // notice that the model is intentionally trained to learns the difference between the distorted output and the clean input, 
        // so the clean sample must be added back after passing it to the forward method in addition to the conditioning parameter
        buffer[sample] = model.forward(parInput) + buffer[sample];   
    }
}

// This function allows to update the drive gain value each time the knob is moved
void NeuralNetwork::setDrive(float value)
{
    driveGain = value;
}

// This function sums to vectors if they have the same length, otherwise it returns an empty vector
floatVec NeuralNetwork::sum(const floatVec& x, const floatVec& y)
{
    auto firstSize = x.size();
    auto secondSize = y.size();
    if (firstSize == secondSize)
    {
        floatVec z(firstSize, 0.0f);
        for (size_t i = 0; i < firstSize; ++i)
        {
            z[i] = x[i] + y[i];
        }
        return z;
    }
    else
    {
        return floatVec();
    }
}

// This function returns the transpose of a generic rectangular matrix
floatVec2D NeuralNetwork::transpose(const floatVec2D& x)
{
    auto outerSize = x.size();
    auto innerSize = x[0].size();
    floatVec2D y(innerSize, floatVec(outerSize, 0.0f));
    for (size_t i = 0; i < outerSize; ++i)
    {
        for (size_t j = 0; j < innerSize; ++j)
            y[j][i] = x[i][j];
    }
    return y;
}