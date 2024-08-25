# OCD Emulator

Hybrid RNN/WDF FullTone OCD emulation coded in JUCE framework.

![GUI](/Assets/Images/OCD_GUI_small.jpg)

## Instructions for recompiling

It is recommended to use VSCode with CMake tools (Release version) installed.

The only line of code that must be changed is found in [ModelPath](/PluginCode/CMakeLists.txt) at line 29: the global path of the .json file containing the trained neural network has to be adapted to the new system.
