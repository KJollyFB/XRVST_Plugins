/*
    JUCE Header for XRVST_Plugins

    This file includes all the JUCE module headers needed for this plugin.
*/

#pragma once

// JUCE Module Configuration
#define JUCE_DISPLAY_SPLASH_SCREEN 0
#define JUCE_USE_DARK_SPLASH_SCREEN 1
#define JUCE_MODULE_AVAILABLE_juce_audio_basics 1
#define JUCE_MODULE_AVAILABLE_juce_audio_devices 1
#define JUCE_MODULE_AVAILABLE_juce_audio_formats 1
#define JUCE_MODULE_AVAILABLE_juce_audio_plugin_client 1
#define JUCE_MODULE_AVAILABLE_juce_audio_processors 1
#define JUCE_MODULE_AVAILABLE_juce_audio_utils 1
#define JUCE_MODULE_AVAILABLE_juce_core 1
#define JUCE_MODULE_AVAILABLE_juce_data_structures 1
#define JUCE_MODULE_AVAILABLE_juce_events 1
#define JUCE_MODULE_AVAILABLE_juce_graphics 1
#define JUCE_MODULE_AVAILABLE_juce_gui_basics 1
#define JUCE_MODULE_AVAILABLE_juce_gui_extra 1

// Plugin settings
#define JucePlugin_Build_VST 0
#define JucePlugin_Build_VST3 1
#define JucePlugin_Build_AU 0
#define JucePlugin_Build_AUv3 0
#define JucePlugin_Build_AAX 0
#define JucePlugin_Build_Standalone 1
#define JucePlugin_Build_Unity 0
#define JucePlugin_Build_LV2 0

#define JucePlugin_Name "Broadband Compressor V2"
#define JucePlugin_Desc "Standard dynamic range compressor with soft-knee curve"
#define JucePlugin_Manufacturer "Meta Audio"
#define JucePlugin_ManufacturerWebsite "https://meta.com"
#define JucePlugin_ManufacturerEmail ""
#define JucePlugin_ManufacturerCode 0x4d657461 // 'Meta'
#define JucePlugin_PluginCode 0x42424332 // 'BBC2'
#define JucePlugin_IsSynth 0
#define JucePlugin_WantsMidiInput 0
#define JucePlugin_ProducesMidiOutput 0
#define JucePlugin_IsMidiEffect 0
#define JucePlugin_EditorRequiresKeyboardFocus 0
#define JucePlugin_Version 1.0.0
#define JucePlugin_VersionCode 0x10000
#define JucePlugin_VersionString "1.0.0"
#define JucePlugin_VSTUniqueID JucePlugin_PluginCode
#define JucePlugin_VSTCategory kPlugCategEffect
#define JucePlugin_Vst3Category "Fx|Dynamics"
#define JucePlugin_AUMainType 'aufx'
#define JucePlugin_AUSubType JucePlugin_PluginCode
#define JucePlugin_AUExportPrefix XRVST_PluginsAU
#define JucePlugin_AUExportPrefixQuoted "XRVST_PluginsAU"
#define JucePlugin_AUManufacturerCode JucePlugin_ManufacturerCode
#define JucePlugin_CFBundleIdentifier com.meta.xrvst-plugins
#define JucePlugin_AAXIdentifier com.meta.xrvst-plugins
#define JucePlugin_AAXManufacturerCode JucePlugin_ManufacturerCode
#define JucePlugin_AAXProductId JucePlugin_PluginCode
#define JucePlugin_AAXCategory 2
#define JucePlugin_AAXDisableBypass 0
#define JucePlugin_AAXDisableMultiMono 0
#define JucePlugin_IAAType 0x61757278 // 'aurx'
#define JucePlugin_IAASubType JucePlugin_PluginCode
#define JucePlugin_IAAName "Meta Audio: Broadband Compressor V2"
#define JucePlugin_VSTNumMidiInputs 16
#define JucePlugin_VSTNumMidiOutputs 16
#define JucePlugin_MaxNumInputChannels 2
#define JucePlugin_MaxNumOutputChannels 2
#define JucePlugin_PreferredChannelConfigurations {1, 1}, {2, 2}

// JUCE configuration
#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#define JUCE_STRICT_REFCOUNTEDPOINTER 1
#define JUCE_VST3_CAN_REPLACE_VST2 0
#define JUCE_WEB_BROWSER 0
#define JUCE_USE_CURL 0

// Include JUCE modules
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
