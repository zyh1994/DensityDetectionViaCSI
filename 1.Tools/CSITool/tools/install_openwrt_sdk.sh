#!/bin/sh

SDK_URL="https://downloads.openwrt.org/snapshots/targets/ath79/generic/openwrt-sdk-ath79-generic_gcc-12.3.0_musl.Linux-x86_64.tar.xz"
SDK_FILE=$(basename $SDK_URL)

# Download the SDK
wget $SDK_URL

# Verify the download
if [ $? -ne 0 ]; then
    echo "Failed to download the SDK"
    exit 1
fi

# Extract the SDK
tar xvf $SDK_FILE

if [ $? -ne 0 ]; then
    echo "Failed to extract the SDK"
    exit 1
fi

echo "SDK downloaded and extracted successfully"
