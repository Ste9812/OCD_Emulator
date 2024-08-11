cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

project(OCD_Emu VERSION 0.0.1)

add_subdirectory(../Dependencies/RTNeural ${CMAKE_CURRENT_BINARY_DIR}/RTNeural)
include_directories(../Dependencies/RTNeural)

set(EIGEN3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../Dependencies/Eigen")
include_directories(${EIGEN3_INCLUDE_DIR})

add_subdirectory(../Dependencies/JUCE ./JUCE)  
include_directories(../Dependencies/JUCE)

juce_add_plugin(OCD_Emu
    COMPANY_NAME PoliMi                        
    IS_SYNTH FALSE                       
    NEEDS_MIDI_INPUT FALSE              
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE        
    PLUGIN_MANUFACTURER_CODE RaSt             
    PLUGIN_CODE Ocde  
    FORMATS AU VST3 Standalone                
    PRODUCT_NAME "OCD_Emu")       

juce_generate_juce_header(OCD_Emu)

target_sources(OCD_Emu
    PRIVATE
    src/PluginEditor.cpp
    src/PluginProcessor.cpp
    src/NeuralNetwork.cpp
    src/ToneControl.cpp
    src/MonoLimiter.cpp)

target_compile_definitions(OCD_Emu
    PUBLIC 
        JUCE_ALSA=1
        JUCE_DIRECTSOUND=1
        JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
        JUCE_USE_OGGVORBIS=1
        JUCE_WEB_BROWSER=0  
        JUCE_USE_CURL=0    
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(OCD_Emu
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_core
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)