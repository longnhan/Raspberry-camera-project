#!/bin/bash

echo "Time build: $(date '+%Y-%m-%d %H:%M')"
echo "User: $USER"

# Define project root and build directory
PROJECT_DIR=$(pwd)/..
echo "Project direcotry is: " $PROJECT_DIR
BUILD_DIR="${PROJECT_DIR}/build"

# Clean previous builds if they exist
echo "Cleaning previous build..."
rm -rf "$BUILD_DIR"

# Create a new build directory at the root of the project
echo "Creating new build directory..."
mkdir -p "$BUILD_DIR"

# Set up environment variables
# export PKG_CONFIG_PATH=${PROJECT_DIR}/tools/sysroot/usr/lib/arm-linux-gnueabihf/pkgconfig:$PKG_CONFIG_PATH
export SYSROOT="${PROJECT_DIR}/tools/sysroot"
export LD=arm-linux-gnueabihf-ld
export CC=arm-linux-gnueabihf-gcc
export CXX=arm-linux-gnueabihf-g++
export CXXFLAGS="--sysroot=${SYSROOT}"
export LDFLAGS="--sysroot=${SYSROOT} -L${SYSROOT}/usr/lib/arm-linux-gnueabihf"

export LIBRARY_PATH="${SYSROOT}/usr/lib"
export LD_LIBRARY_PATH="${SYSROOT}/usr/lib"

# Run CMake to configure the project
echo "Configuring the project using CMake..."
cd "$BUILD_DIR" || exit 1
cmake -DCMAKE_TOOLCHAIN_FILE="${PROJECT_DIR}/toolchain.cmake" ..

# Build the project
echo "Building the project..."
make -j$(nproc)  VERBOSE=1

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build completed successfully!"
else
    echo "Build failed. Please check the errors above."
    exit 1
fi

echo ::::----------------------------::::
echo ::::-----------Finish-----------::::
echo ::::----------------------------::::
