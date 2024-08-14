#include "ToneControl.h"

//==============================================================================
ToneControl::ToneControl()
{
    B_V << -1, 0,  0,  0, 0,  0, 0, 1, 0, 0, 0, 
            0, 0,  0, -1, 1,  0, 0, 0, 1, 0, 0, 
            1, 1, -1,  1, 0, -1, 0, 0, 0, 1, 0, 
            0, 0, -1,  0, 0, -1, 1, 0, 0, 0, 1;
    B_I << -1, 0,  0, 0, 0,  0, 0, 1, 0, 0, 0,
            1, 0,  0, 0, 1,  0, 0, 0, 1, 0, 0,
            0, 1, -1, 0, 0, -1, 0, 0, 0, 1, 0,
            0, 0, -1, 0, 0, -1, 1, 0, 0, 0, 1;
    Rsw = 1.0 / (1.0 / R11 + 1.0 / R12);
    Rtc = Rtol;
    Rva = 0.1 * Rv + Rtol;
    Rvb = 0.9 * Rv + Rtol;
}

//==============================================================================
void ToneControl::prepare(double sampleRate)
{
    double Z_g = Rg;
    double Z_R9 = R9;
    double Z_R10 = R10;
    double Z_Rsw = Rsw;
    double Z_Rtc = Rtc;
    double Z_Rva = Rva;
    double Z_Rvb = Rvb;
    double Z_C5 = 1.0 / (2.0 * C5 * sampleRate);
    double Z_C6 = 1.0 / (2.0 * C6 * sampleRate);
    double Z_C7 = 1.0 / (2.0 * C7 * sampleRate);
    double Z_C8 = 1.0 / (2.0 * C8 * sampleRate);
    Z = (colVec() << Z_C5 , Z_C7 , Z_C8 , Z_g , Z_C6 , Z_Rtc , Z_Rva , Z_R9 , Z_R10 , Z_Rsw , Z_Rvb).finished().asDiagonal();
    updateS();
}

//==============================================================================
void ToneControl::process(float* buffer, int bufferLength)
{
    for (auto sample = 0; sample < bufferLength; ++sample)
    {
        b(0) = a(0);
        b(1) = a(1);
        b(2) = a(2);
        b(3) = double(buffer[sample]);
        b(4) = a(4);
        b(5) = 0.0;
        b(6) = 0.0;
        b(7) = 0.0;
        b(8) = 0.0;
        b(9) = 0.0;
        b(10) = 0.0;
        a = S * b;
        buffer[sample] = float((a(6) + b(6)) / 2.0);
    }   
}

//==============================================================================
void ToneControl::setSwitch(bool state)
{
    if (state)
    {
        Rsw = R11;
    }
    else
    {
        Rsw = 1.0 / (1.0 / R11 + 1.0 / R12);
    }
    Z(9, 9) = Rsw;
    updateS();
}

void ToneControl::setTone(double value)
{
    Rtc = Rt * value + Rtol;
    Z(5, 5) = Rtc;
    updateS();
}

void ToneControl::setVolume(double value)
{
    Rva = Rv * (A * pow(B, value) + C) + Rtol;
    Rvb = Rv * (1 - A * pow(B, value) - C) + Rtol;
    Z(6, 6) = Rva;
    Z(10, 10) = Rvb;
    updateS();
}

//==============================================================================
void ToneControl::updateS()
{
    S = sqMat::Identity() - 2 * Z * B_I.transpose() * (B_V * Z * B_I.transpose()).inverse() * B_V;
}