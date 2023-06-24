#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

if [ ! -d "linux-80211n-csitool" ]; then
    echo "${GREEN}Cloning the modified Linux kernel code...${NC}"
    git clone https://github.com/seagochen/linux-80211n-csitool.git
fi

if [ ! -d "linux-80211n-csitool-supplementary" ]; then
    echo "${GREEN}Obtaining the CSI Tool supplementary material...${NC}"
    git clone https://github.com/seagochen/linux-80211n-csitool-supplementary.git
fi

if [ ! -d "CSI-Data" ]; then
    echo "${GREEN}Cloning the modified Linux kernel code...${NC}"
    git clone https://github.com/Gi-z/CSI-Data.git
fi