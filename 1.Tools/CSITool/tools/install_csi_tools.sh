#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Check if the kernel version
echo "${GREEN}Checking kernel version...${NC}"
uname -r

# Install required packages
echo "${GREEN}Installing required packages...${NC}"
sudo apt install build-essential linux-headers-$(uname -r) git-core

# Add the PPA for the target GCC and G++ compilers
echo "${GREEN}Installing compilers with retpoline support...${NC}"
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install gcc-8 g++-8

# Install the target GCC and G++ compilers
echo "${GREEN}Redirecting system-wide links to the compiler executables...${NC}"
sudo rm /usr/bin/gcc
sudo rm /usr/bin/g++
sudo ln -s /usr/bin/gcc-8 /usr/bin/gcc
sudo ln -s /usr/bin/g++-8 /usr/bin/g++

# Modify the kernel. 
# If the directory is not exist, do the following
if [ ! -d "linux-80211n-csitool" ]; then
    echo "${GREEN}Cloning the modified Linux kernel code...${NC}"
    #git clone https://github.com/spanev/linux-80211n-csitool.git
    git clone https://github.com/seagochen/linux-80211n-csitool.git
fi
cd linux-80211n-csitool

# Checkout the correct release version
echo "${GREEN}Checking out the correct release version...${NC}"
CSITOOL_KERNEL_TAG=csitool-$(uname -r | cut -d . -f 1-2)
git checkout ${CSITOOL_KERNEL_TAG}

# Build the modified kernel
echo "${GREEN}Building the modified driver for the existing kernel...${NC}"
make -j `nproc` -C /lib/modules/$(uname -r)/build M=$(pwd)/drivers/net/wireless/intel/iwlwifi modules
sudo make -C /lib/modules/$(uname -r)/build M=$(pwd)/drivers/net/wireless/intel/iwlwifi INSTALL_MOD_DIR=updates modules_install

# Install the modified kernel
echo "${GREEN}Running depmod...${NC}"
sudo depmod
cd ..

# Install the CSI Tool
# if the directory is not exist, do the following
if [ ! -d "linux-80211n-csitool-supplementary" ]; then
    echo "${GREEN}Obtaining the CSI Tool supplementary material...${NC}"
    #git clone https://github.com/dhalperi/linux-80211n-csitool-supplementary.git
    git clone https://github.com/seagochen/linux-80211n-csitool-supplementary.git
fi

# Install the modified firmware
echo "${GREEN}Relocating any existing firmware for Intel Wi-Fi Link 5000 Series adapters...${NC}"
for file in /lib/firmware/iwlwifi-5000-*.ucode; do sudo mv $file $file.orig; done

echo "${GREEN}Installing the modified firmware...${NC}"
sudo cp linux-80211n-csitool-supplementary/firmware/iwlwifi-5000-2.ucode.sigcomm2010 /lib/firmware/
sudo ln -s iwlwifi-5000-2.ucode.sigcomm2010 /lib/firmware/iwlwifi-5000-2.ucode

# Build the logging tool
echo "${GREEN}Build the logging tool"
make -C linux-80211n-csitool-supplementary/netlink

# Create a symbolic link to the netlink tools
# If the link is not exist, do the following
if [ ! -L "netlink" ]; then
    echo "${GREEN}Create a symbolic link to the logging tool"
    sudo ln -s $(pwd)/linux-80211n-csitool-supplementary/netlink ./netlink
fi

# Message the completion of the installation
echo "${GREEN}Done.${NC}"