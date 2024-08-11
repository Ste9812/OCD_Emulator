#include "NeuralNetwork.h"

//==============================================================================
NeuralNetwork::NeuralNetwork(const char* filePath)
{
    this->loadWeights(filePath);
}

//==============================================================================
void NeuralNetwork::loadWeights(const char* filePath)
    {
        std::ifstream jsonStream(filePath);
        nlohmann::json weightsJson;
        jsonStream >> weightsJson;
        auto& lstm = model.template get<0>();
        auto& dense = model.template get<1>();
        floatVec2D weights_ih = weightsJson["/state_dict/rec.weight_ih_l0"_json_pointer];
        lstm.setWVals(transpose(weights_ih));
        floatVec2D weights_hh = weightsJson["/state_dict/rec.weight_hh_l0"_json_pointer];
        lstm.setUVals(transpose(weights_hh));
        floatVec bias_ih = weightsJson["/state_dict/rec.bias_ih_l0"_json_pointer];
        floatVec bias_hh = weightsJson["/state_dict/rec.bias_hh_l0"_json_pointer];
        lstm.setBVals(sum(bias_ih, bias_hh));
        floatVec2D lin_weights = weightsJson["/state_dict/lin.weight"_json_pointer];
        dense.setWeights(lin_weights);
        floatVec lin_bias = weightsJson["/state_dict/lin.bias"_json_pointer];
        dense.setBias(lin_bias.data());
        model.reset();  
    }

//==============================================================================
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

//==============================================================================
void NeuralNetwork::process(float* buffer, int bufferLength)
{
    for (int sample = 0; sample < bufferLength; ++sample)
    {
        parInput[0] = buffer[sample];
        parInput[1] = driveGain;
        buffer[sample] = model.forward(parInput) + buffer[sample];
    }
}

//==============================================================================
void NeuralNetwork::setDrive(float value)
{
    driveGain = value;
}

//==============================================================================
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