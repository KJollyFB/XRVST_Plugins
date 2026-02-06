cmake_minimum_required(VERSION 3.22)

project(XRVST_Plugins VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JUCE - update this path to your JUCE installation
# You can set JUCE_DIR as an environment variable or CMake variable
if(NOT DEFINED JUCE_DIR)
    # Try to find JUCE in common locations
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE")
        set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE")
    elseif(EXISTS "/usr/local/JUCE")
        set(JUCE_DIR "/usr/local/JUCE")
    else()
        message(FATAL_ERROR "JUCE_DIR not set. Please set JUCE_DIR to your JUCE installation path.")
    endif()
endif()

add_subdirectory(${JUCE_DIR} ${CMAKE_BINARY_DIR}/JUCE)

# Plugin target: Broadband Compressor V2
juce_add_plugin(BroadbandCompressorV2
    COMPANY_NAME "Meta Audio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Meta
    PLUGIN_CODE BBC2
    FORMATS VST3 Standalone
    PRODUCT_NAME "Broadband Compressor V2"
)

# Source files
target_sources(BroadbandCompressorV2
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.h
        Source/Parameters.h
        Source/DSP/Utilities.h
        Source/DSP/LevelEstimator.h
        Source/DSP/CompressionCurve.h
        Source/DSP/DelayLine.h
        Source/GUI/GainReductionMeter.h
        Source/GUI/TransferCurve.h
        Source/GUI/CustomLookAndFeel.h
)

# Include directories
target_include_directories(BroadbandCompressorV2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# Compile definitions
target_compile_definitions(BroadbandCompressorV2
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Link JUCE modules
target_link_libraries(BroadbandCompressorV2
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compiler-specific settings
if(MSVC)
    target_compile_options(BroadbandCompressorV2 PRIVATE /W4)
else()
    target_compile_options(BroadbandCompressorV2 PRIVATE -Wall -Wextra -Wpedantic)
endif()
