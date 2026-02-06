#!/bin/bash
#
# Build script for XRVST_Plugins
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Set JUCE_DIR to your local JUCE installation path, or pass it as an argument:
#   ./build.sh /path/to/JUCE
if [ -n "$1" ]; then
    JUCE_DIR="$1"
elif [ -n "$JUCE_DIR" ]; then
    : # Use existing environment variable
elif [ -d "${SCRIPT_DIR}/../JUCE" ]; then
    JUCE_DIR="${SCRIPT_DIR}/../JUCE"
else
    echo "Error: JUCE_DIR not set."
    echo "Usage: ./build.sh /path/to/JUCE"
    echo "   or: export JUCE_DIR=/path/to/JUCE && ./build.sh"
    exit 1
fi
BUILD_DIR="${SCRIPT_DIR}/build"

echo "=========================================="
echo "XRVST_Plugins - Build Script"
echo "=========================================="

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo ""
    echo "CMake not found. Installing via Homebrew..."
    brew install cmake
fi

echo ""
echo "Using JUCE from: ${JUCE_DIR}"
echo "Build directory: ${BUILD_DIR}"
echo ""

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake
echo "Configuring project..."
cmake .. -DJUCE_DIR="${JUCE_DIR}" -DCMAKE_BUILD_TYPE=Release

# Build
echo ""
echo "Building plugins..."
# Detect CPU count cross-platform
if [ "$(uname)" = "Darwin" ]; then
    NPROC=$(sysctl -n hw.ncpu)
else
    NPROC=$(nproc 2>/dev/null || echo 4)
fi
cmake --build . --config Release -j"${NPROC}"

echo ""
echo "=========================================="
echo "Build complete!"
echo "=========================================="
echo ""

# Find the built plugin
VST3_PATH=$(find "${BUILD_DIR}" -name "*.vst3" -type d 2>/dev/null | head -1)
if [ -n "$VST3_PATH" ]; then
    echo "VST3 plugin built at:"
    echo "  ${VST3_PATH}"
    echo ""
    echo "To install, copy to your VST3 folder:"
    echo "  cp -r \"${VST3_PATH}\" ~/Library/Audio/Plug-Ins/VST3/"
fi
