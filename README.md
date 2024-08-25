# OCD Emulator

Hybrid RNN/WDF FullTone OCD emulation coded in JUCE framework.

![GUI](/Assets/Images/OCD_GUI_small.jpg)

## Instructions for Recompiling

It is recommended to use VSCode with CMake tools (Release version) installed.

The vst3 files and the executable were built with Visual Studio Community 2022 Release - amd64 kit on a Windows-based platform.

The only line of code that must be changed is found in [CMakeLists.txt](/PluginCode/CMakeLists.txt) at line 29: the global path of the .json file containing the trained neural network has to be adapted to the new system.

If you desire to load a model with a different hidden size, it would be sufficient to modify the line 6 of the [NeuralNetwork.h](/PluginCode/src/Processing/NeuralNetwork.h) file.

## Presentation and Documentation

The technical report, the video presentation and the auxiliary slides are located in the [Assets folder](/Assets) of this repository.

## Compiled VST and Executable

The compiled VST plugin (in .vst3 and .exe format) are located in the [Compiled](/Compiled) folder of this repository.

## Aknowledgments

Alec Wright - Python training code

Keith Bloemer - GuitarML page

Alessandro Proverbio - Neural network class management

Luca Vitale - Live demonstration
