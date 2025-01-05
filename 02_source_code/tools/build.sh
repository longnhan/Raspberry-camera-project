#!/bin/bash

echo "Time build: $(date '+%Y-%m-%d %H:%M')"
echo "User: $USER"

# Define project root and build directory
PROJECT_DIR=$(pwd)/..
BUILD_DIR="${PROJECT_DIR}/build"

# Clean previous builds if they exist
echo "Cleaning previous build..."
rm -rf "$BUILD_DIR"

# Create a new build directory at the root of the project
echo "Creating new build directory..."
mkdir -p "$BUILD_DIR"

# Run CMake to configure the project
echo "Configuring the project using CMake..."
cd "$BUILD_DIR" || exit 1
cmake ..

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

echo ::::----------------------------::::
echo ::::-----------Finish-----------::::
echo ::::----------------------------::::