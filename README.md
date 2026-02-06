# XRVST_Plugins - VST3 Plugin Collection

A collection of professional-grade VST3 audio plugins for XR audio applications, based on Meta's FBAudio algorithms.

## Plugins

### Broadband Compressor V2

A professional-grade dynamic range compressor VST3 plugin, based on Meta's FBAudio Broadband Compressor V2 algorithm.

#### Features

- **Soft-Knee Compression**: Smooth transition from linear to compressed regions using quadratic curves
- **Envelope Follower**: Configurable attack, release, and hold times for precise dynamics control
- **Lookahead**: Up to 30ms of lookahead for transient-aware compression
- **Linked Stereo Mode**: Preserve stereo image by using max level across channels
- **Real-time Visualization**:
  - Transfer curve display showing compression characteristic
  - Gain reduction meter with peak hold

#### Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Linked | On/Off | On | Link channels for stereo image preservation |
| Bypass | On/Off | Off | Bypass processing |
| Input Gain | -30 to +30 dB | 0 dB | Pre-compression gain |
| Threshold | -96 to 0 dB | 0 dB | Compression threshold |
| Ratio | 1:1 to 100:1 | 1:1 | Compression ratio |
| Knee Width | 0 to 48 dB | 0 dB | Soft knee transition width |
| Output Gain | -30 to +30 dB | 0 dB | Makeup gain |
| Attack | 0 to 1000 ms | 0 ms | Attack time constant |
| Release | 0 to 1000 ms | 100 ms | Release time constant |
| Hold | 0 to 1000 ms | 0 ms | Hold time before release |
| Lookahead | 0 to 30 ms | 0 ms | Lookahead delay time |

#### Algorithm

The compression algorithm implements a three-region soft-knee characteristic:

1. **Below Knee** (input < threshold - knee/2): No compression, unity gain
2. **In Knee** (within knee region): Quadratic interpolation for smooth transition
3. **Above Knee** (input > threshold + knee/2): Constant ratio compression

The envelope follower uses separate attack and release coefficients with optional hold time:
- Attack: Ramps up quickly when signal exceeds current level
- Hold: Maintains level for specified duration
- Release: Ramps down gradually when signal decreases

## Building

### Prerequisites

- JUCE 8.0.8 or later
- CMake 3.22 or later
- C++17 compatible compiler:
  - macOS: Xcode 13+
  - Windows: Visual Studio 2022

### Building with CMake

```bash
# Clone or download JUCE to a location, e.g., ~/JUCE

# Configure
mkdir build && cd build
cmake .. -DJUCE_DIR=~/JUCE

# Build
cmake --build . --config Release
```

### Building with Projucer

1. Open `XRVST_Plugins.jucer` in Projucer
2. Set the JUCE modules path in Projucer preferences
3. Export to your IDE (Xcode or Visual Studio)
4. Build the project

## Project Structure

```
XRVST_Plugins/
├── Source/
│   ├── DSP/
│   │   ├── Utilities.h         # dB/magnitude conversions
│   │   ├── LevelEstimator.h    # Envelope follower
│   │   ├── CompressionCurve.h  # Soft-knee algorithm
│   │   └── DelayLine.h         # Lookahead buffer
│   ├── GUI/
│   │   ├── GainReductionMeter.h    # VU-style meter
│   │   ├── TransferCurve.h         # Curve visualization
│   │   └── CustomLookAndFeel.h     # Dark theme
│   ├── Parameters.h            # Parameter definitions
│   ├── PluginProcessor.h/cpp   # Audio processing
│   └── PluginEditor.h/cpp      # GUI
├── CMakeLists.txt              # CMake build config
└── XRVST_Plugins.jucer         # Projucer project
```

## Dependencies

This project uses the [JUCE framework](https://juce.com/) (version 8.0.8+), which is available under a dual license:
- **GPLv3** — Free for open-source projects
- **Commercial License** — Required for closed-source/commercial use

JUCE is **not** bundled with this repository. You must download or clone JUCE separately and point the build system to it via the `JUCE_DIR` variable.

## License

Copyright (c) 2024 Meta. Based on FBAudio library algorithms.

## Credits

Original algorithm: Meta FBAudio Team
VST plugin implementation: Based on JUCE framework
