#!/bin/bash

# Download GDB source code

# if the file is already downloaded, skip this step
if [ -f gdb-10.2.tar.xz ]; then
    echo "gdb-10.2.tar.xz already exists, skip downloading"
else
    wget https://ftp.gnu.org/gnu/gdb/gdb-10.2.tar.xz
fi
tar xf gdb-10.2.tar.xz
cd gdb-10.2

# Install build dependencies
sudo apt update
sudo apt install -y build-essential texinfo bison flex libncurses5-dev

# Configure and build GDB
./configure
make -j4

# Install GDB
sudo make install

# Verify the installation
gdb --version

# Clean up
cd ..
#rm -rf gdb-10.2 gdb-10.2.tar.xz
