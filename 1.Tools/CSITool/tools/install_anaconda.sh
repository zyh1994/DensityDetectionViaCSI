#!/bin/bash

ANACONDA_VERSION=5.0.1
ANACONDA_INSTALLER=Anaconda3-$ANACONDA_VERSION-Linux-x86_64.sh
ANACONDA_URL=https://repo.anaconda.com/archive/$ANACONDA_INSTALLER

# Download Anaconda installer
echo "Downloading Anaconda installer..."
wget $ANACONDA_URL

# Verify the integrity of the downloaded installer
echo "Verifying installer integrity..."
sha256sum $ANACONDA_INSTALLER

# Prompt the user to confirm the integrity of the downloaded installer
read -p "Do you want to continue with the installation? (y/n) " answer
if [[ "$answer" != "y" ]]; then
    echo "Installation canceled."
    exit 1
fi

# If ANACONDA_INSTALLER is exist, do the following
if [ -f "$ANACONDA_INSTALLER" ]; then
    echo "Installing Anaconda..."
    bash $ANACONDA_INSTALLER -b

    # Add Anaconda to PATH
    echo "Adding Anaconda to PATH..."
    echo 'export PATH="~/anaconda3/bin:$PATH"' >> ~/.bashrc
    source ~/.bashrc

    # Echo the completion of the installation
    echo "Anaconda installation complete!"

    # Remove the installer
    rm $ANACONDA_INSTALLER
else
    echo "$ANACONDA_INSTALLER does not exist."
    exit 1
fi


