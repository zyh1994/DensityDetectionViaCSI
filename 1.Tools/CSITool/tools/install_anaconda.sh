#!/bin/bash

ANACONDA_VERSION=5.0.1
ANACONDA_INSTALLER=Anaconda3-$ANACONDA_VERSION-Linux-x86_64.sh
ANACONDA_URL=https://repo.anaconda.com/archive/$ANACONDA_INSTALLER
ANACONDA_HASH_VAL=55e4db1919f49c92d5abbf27a4be5986ae157f074bf9f8238963cd4582a4068a
ANACONDA_INSTALL_DIR=$HOME/anaconda3

# Create a temporary directory for the installer
TEMP_DIR=$(mktemp -d)

# Download Anaconda installer
echo "Downloading Anaconda installer..."
cd $TEMP_DIR || exit 1
wget -q $ANACONDA_URL

# Verify the integrity of the downloaded installer
echo "Verifying installer integrity..."
if [ "$(sha256sum $ANACONDA_INSTALLER | awk '{print $1}')" != "$ANACONDA_HASH_VAL" ]; then
    echo "ERROR: Hash code does not match!"
    exit 1
else
    echo "The Anaconda installer file is downloaded and verified."
fi

# Prompt the user to confirm the integrity of the downloaded installer
read -p "Do you want to continue with the installation? (y/n) " answer
if [ "$answer" != "y" ]; then
    echo "Installation canceled."
    exit 1
fi

# Install Anaconda
echo "Installing Anaconda..."
bash $ANACONDA_INSTALLER -b -p "$ANACONDA_INSTALL_DIR"

# Add Anaconda to PATH
echo "Adding Anaconda to PATH..."
echo 'export PATH="$ANACONDA_INSTALL_DIR/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc

# Echo the completion of the installation
echo "Anaconda installation complete!"

# Clean up the temporary directory
cd ~
trap "rm -rf $TEMP_DIR" EXIT
