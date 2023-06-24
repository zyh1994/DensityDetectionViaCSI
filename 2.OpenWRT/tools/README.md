# What to do for this package?

First, the OpenWRT SDK needs python environment, therefore you have to install the target version of python on your system.

You can install anaconda by using install_anaconda.sh in the `1.Intel5300/CSITool/tools/install_anaconda.sh`. Then, activate the virtual environment by 

```bash
$ source activate csi
```

Then, perform the following command to download the target device SDK.

```bash
$ sh download_sdk.sh
```

## Update the environment of SDK

To use the following command to update the environment

```bash
$ ./scripts/feeds update -a
```

Then install all the updates

```bash
./scripts/feeds install -a
```

After that, perform the following command to configure the environment.

```bash
make menuconfig
```

## Prepare the OpenWRT SDK

Copy the target projects into the `package` folder of the SDK. Then, perform the following example to compile the project.

```bash
make package/csi_udp_resender/compile V=sc
```