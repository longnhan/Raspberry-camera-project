#!/bin/bash

echo "Time build: $(date '+%Y-%m-%d %H:%M')"
echo "User: $USER"

# Ask the user to choose between cross-compilation and native compilation
echo "Select build option:"
echo "1) Cross-compile (Build on PC for Raspberry Pi)"
echo "2) Native compile (Build directly on Raspberry Pi)"
read -p "Enter choice (1 or 2): " BUILD_OPTION

# Define project root and build directory
PROJECT_DIR=$(pwd)/..
BUILD_DIR="${PROJECT_DIR}/build"

# Ask the user whether to clean the previous build
read -p "Do you want to perform a clean build? (y/n): " CLEAN_BUILD

if [ "$CLEAN_BUILD" == "y" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
else
    echo "Keeping previous build..."
fi

# Create a new build directory if it doesn't exist
echo "Creating new build directory..."
mkdir -p "$BUILD_DIR"

# Set up environment variables
if [ "$BUILD_OPTION" == "1" ]; then
    echo "Selected: Cross-compilation"
    # SYSROOT="${PROJECT_DIR}/tools/sysroot"
    TOOLCHAIN_FILE="${PROJECT_DIR}/toolchain.cmake"
    # export CC=arm-linux-gnueabihf-gcc
    export CXX=arm-linux-gnueabihf-g++
    # export CXXFLAGS="--sysroot=${SYSROOT}"
    # export LDFLAGS="--sysroot=${SYSROOT} -L${SYSROOT}/usr/lib/arm-linux-gnueabihf"

elif [ "$BUILD_OPTION" == "2" ]; then
    echo "Selected: Native compilation"
    TOOLCHAIN_FILE=""  # No toolchain file needed for native build
else
    echo "Invalid option. Please select 1 or 2."
    exit 1
fi

# Run CMake to configure the project
echo "Configuring the project using CMake..."
cd "$BUILD_DIR" || exit 1
cmake -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" ..

# Build the project
echo "Building the project..."
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build completed successfully!"
else
    echo "Build failed. Please check the errors above."
    exit 1
fi

echo "::::----------------------------::::"
echo "::::-----------Finish-----------::::"
echo "::::----------------------------::::"
