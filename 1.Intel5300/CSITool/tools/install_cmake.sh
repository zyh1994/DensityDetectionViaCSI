#!/bin/bash

# Update package list and install necessary packages
sudo apt-get update
sudo apt-get install -y build-essential libssl-dev

# Download and extract CMake 3.10
CMAKE_VERSION="3.10.0"
wget "https://cmake.org/files/v3.10/cmake-${CMAKE_VERSION}.tar.gz"
tar -xvf "cmake-${CMAKE_VERSION}.tar.gz"
cd "cmake-${CMAKE_VERSION}"

# Build and install CMake
./bootstrap
make -j$(nproc)
sudo make install

# Clean up
cd ..
rm -rf "cmake-${CMAKE_VERSION}"
rm "cmake-${CMAKE_VERSION}.tar.gz"

# Verify installation
cmake --version

# Make a shortcut to /usr/bin
sudo ln -s /usr/local/bin/cmake /usr/bin/cmake

echo "CMake ${CMAKE_VERSION} installation completed."

