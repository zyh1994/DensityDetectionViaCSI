
# 1. Download and install Ubuntu 16.04.7 with kernel 4.15.0-112-generic:

You can use Rufus to create a bootable USB drive with the Ubuntu 16.04.7 ISO file. Refer to this guide for instructions on creating the bootable USB drive. Alternatively, you can use UltraISO to create a bootable USB drive.

# 2. Change the Ubuntu mirror and disable updates:

After installing Ubuntu, open a terminal and run the following command to update the package repository index:

```bash
sudo apt-get update
```

Then, run the following command to upgrade the installed packages:

```bash
sudo apt-get upgrade
```

Next, change the Ubuntu mirror to an appropriate one by editing the /etc/apt/sources.list file.

To disable updates, run the following command:

```bash
sudo apt-mark hold linux-generic linux-image-generic linux-headers-generic linux-signed-generic
```

This will prevent the kernel from being updated, which is necessary for the CSI tool to work properly.

# 3. Install necessary packages:

Run the following command to install the necessary packages:

```bash
sudo apt install build-essential linux-headers-$(uname -r) git-core
```

This will install the GNU C and C++ compilers, Linux kernel headers for the current kernel, and Git.

Verify that the correct versions of GCC and G++ are installed by running the following command:

```bash
ls -l /usr/bin/gcc /usr/bin/g++
```

The output should show the versions of GCC and G++ installed.

# 4. Build and install modified wireless driver:

Clone the modified Linux kernel code from the CSI tool GitHub repository by running the following command:

```bash
git clone https://github.com/spanev/linux-80211n-csitool.git
```

Change to the cloned repository directory by running the following command:

```bash
cd linux-80211n-csitool
```

Check the correct release version by running the following command:

```bash
CSITOOL_KERNEL_TAG=csitool-$(uname -r | cut -d . -f 1-2)
git checkout ${CSITOOL_KERNEL_TAG}
```

This will check out the version of the kernel that corresponds to the installed version of Ubuntu.

Build the modified wireless driver by running the following command:

```bash
make -j `nproc` -C /lib/modules/$(uname -r)/build M=$(pwd)/drivers/net/wireless/intel/iwlwifi modules
```

Install the modified wireless driver by running the following command:

```bash
sudo make -C /lib/modules/$(uname -r)/build M=$(pwd)/drivers/net/wireless/intel/iwlwifi INSTALL_MOD_DIR=updates modules_install
```

Ignore any SSL-related errors that may occur during the installation.

Clone the CSI tool supplementary code by running the following command:

```bash
git clone https://github.com/dhalperi/linux-80211n-csitool-supplementary.git
```

Move any existing iwlwifi-5000-*.ucode files to a backup location by running the following command:

```bash
for file in /lib/firmware/iwlwifi-5000-*.ucode; do sudo mv $file $file.orig; done
```

Copy the modified firmware file by running the following command:

```bash
sudo cp linux-80211n-csitool-supplementary/firmware/iwlwifi-5000-2.ucode.sigcomm2010 /lib/firmware/
```

Create a symbolic link to the modified firmware file by running the following command:

```bash
sudo ln -s iwlwifi-5000-2.ucode.sigcomm2010 /lib/firmware/iwlwifi-5000-2.ucode
```

This will create a symbolic link to the modified firmware file and allow the wireless card to use it.

Build the netlink tool by running the following command:

```bash
make -C linux-80211n-csitool-supplementary/netlink
```

This will build the tool that will be used to log CSI data to a file.

# 5. Verify CSI tool is working:

To verify that the CSI tool is working, first connect your computer to a wireless network with no password.

Then, in a terminal window, run the following command to unload the iwlwifi module and reload it with the connector_log option:

```bash
sudo modprobe -r iwlwifi mac80211 && sudo modprobe iwlwifi connector_log=0x1
```

Next, run the following command to log the CSI data to a file named csi.dat:

```bash
sudo linux-80211n-csitool-supplementary/netlink/log_to_file csi.dat
```

In another terminal window, find the IP address of the wireless router by opening the network connection information. Then, run the following command to ping the router's IP address and log the CSI data:

```bash
ping <router's IP address> -i 1
```

This will ping the router every second and log the CSI data to the csi.dat file. To stop logging, press Ctrl+C in the terminal running the ping command.

After logging CSI data, you can use the CSI tool MATLAB script to analyze the data and obtain useful information about the wireless environment.