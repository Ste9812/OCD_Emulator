#pragma once
#include <Eigen/Dense>

using rectMat = Eigen::Matrix<double, 4, 11>;
using sqMat = Eigen::Matrix<double, 11, 11>;
using colVec = Eigen::Matrix<double, 11, 1>;

class ToneControl
{
public:
    ToneControl();

    void prepare(double sampleRate);

    void process(float* buffer, int bufferLength);

    void setSwitch(bool state);
    void setTone(double value);
    void setVolume(double value);

private:
    // Logarithmic potentiometer parameters
    double A = 125e-4;
    double B = 81;
    double C = -125e-4;

    // Minimum resistance value to address potentiometer's extreme positions
    double Rtol = 1e-6;

    // Tone control circuital elements characteristic values
    double Rg = 1e-9;
    double R9 = 150e3;
    double R10 = 39e3;
    double R11 = 33e3;
    double R12 = 22e3;
    double Rt = 10e3;
    double Rv = 500e3;
    double C5 = 220e-12;
    double C6 = 100e-9;
    double C7 = 1e-6;
    double C8 = 47e-9;

    // Resistance values that can vary due to potentiometers/switches
    double Rsw;
    double Rtc;
    double Rva;
    double Rvb;

    // Voltage/current newtork fundamental-loop matrix
    rectMat B_V;
    rectMat B_I;

    // Reference port resistances matrix
    sqMat Z;

    // Scattering matrix
    sqMat S;
    
    // Incident/reflected voltage wave vectors
    colVec a;
    colVec b;

    void updateS();
};