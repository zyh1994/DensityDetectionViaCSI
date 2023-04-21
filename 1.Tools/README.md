# 关于该工程的一些说明

## 环境要求

* Linux环境
* 支持Intel 5300
* Python 3.8，并已安装了 requirements.txt 中的全部依赖库，或手动安装以下内容

> csikit
> numpy
> scipy
> matplotlib
> keyboard


## 文件结构

```bash
+-- plotter
    |-- CSIUtilities.py         # 一些代码中的通用模块
    |-- plotter_base.py         # CSI数据绘制基础模块
    |-- __init__.py             # Python模块要求添加的默认文件
    |-- Bfee.py                 # Intel CSI信号解析格式
    |-- resender.py             # 用于测试模拟CSI信号发送的代码
    |-- plotter_replayer.py     # 可用于绘制离线CSI文件的代码
    |-- plotter_online.py       # 可用于绘制在线CSI数据的代码
```

在plotter文件夹下主要使用的是 plotter_replayer 和 plotter_online，前者可以解析 csi.dat 数据，并将离线数据以每秒5帧绘制波形图（绘制速率可调）。plotter_online 可用于接收实时的CSI数据，并可以每秒最快100帧绘制波形图。

```bash
+-- tools
    |-- csi_listener.py         # 当网卡驱动安装好后，可以使用该脚本启动CSI信号监听
    |-- download_thirds.sh      # 该脚本会下载与Intel 5300有关的第三方软件包，但不会执行安装操作
    |-- get_adapter_info.sh     # 获取当前HOST的全部有用网卡信息
    |-- install_csi_tools.sh    # 自动安装5300的驱动
    |-- install_anaconda.sh     # 自动安装Anacoda，该工具是Python用的虚拟环境，用于与其他Python环境隔离避免依赖冲突
    |-- install_cuda_10.sh      # 自动安装CUDA 10
    |-- install_tshark.sh       # 自动安装tshark，该工具可用于网络数据包的格式转换，目前还未用到
```

关于 tools 这部分，是本工具的主要部分。它包含自动对官方工具包的编译、以及自动启动5300网卡，以及收发CSI信号。


# 使用方法

## 如何安装Intel 5300的CSI驱动软件

你可以按照官方的说明一步步的手工安装，也可以通过tools/install_csi_tools.sh 脚本进行自动化安装。安装之前，请确保系统为16.04或者18.04，内核版本为4.15的Ubuntu系统，并确保网络连接畅通，可访问Github，并且已经在电脑中安装了Intel 5300网卡。

在tools目录下，执行以下命令

```bash
$ sh install_csi_tools.sh
```

顺利安装结束后，就可以执行以下内容

## 准备CSI专用环境

目前的一些测试，绘图工具基于Python开发，有些工具需要基于Python 3.6+才能使用，而操作系统自带的Python版本是3.5或以下，所以我们需要安装虚拟环境来隔绝操作系统对我们的影响，因此在本项目中使用到了Anacoda。

Anacoda的安装，也请执行自动脚本
```bash
$ sh install_anaconda.sh
```

安装结束后，为CSI工程创建专用Python虚拟环境，请在Bash中执行以下命令

```bash
$ conda create --name csi python=3.8
```

虚拟环境准备完毕后，启动csi环境

```bash
$ conda activate csi
```

根据系统不同，例如在Linux上，可能启动方式为

```bash
$ source activate csi
```

之后BASH会变成

```bash
$(csi)
```

在 requirements.txt 所在目录层级，执行以下命令，自动安装所需全部依赖包或工具

```bash
$(csi) pip3 install -r requirements.txt
```

等待安装完毕后就可以启动CSI信号监听了。

## 启动CSI信号监听

推荐使用tools/csi_listener.py启动监听

```bash
$(csi) python3 csi_listener.py
```

启动后会首先列出设备中可用的网络设备名称，例如

* wlan01
* wifi01
* vpn01

> 你需要知道自己的5300网卡所对应的设备名称，如果不知道可以使用
>
> ```bash
> $(csi) sh get_adapter_info.sh
> ```
>
> 获取网络设备的全部信息，然后自己判断正确的名字。

选择好设备后，接下来会提示

* Log to file
* Transmit

选择 Log to file 会启动离线记录模式，会将WI-FI接收到的CSI信息直接以日志形式记录，并在当前文件夹下生成一个 csi.dat 的文件。该文件可以被 csikit 工具解析，也可以被 plotter/plotter_replayer.py 或者 Bfee 文件解析。

如果选择 Transmit，会启动一个TCP服务器，如果配合使用 plotter/plotter_online.py，那么就可以把得到的数据实时绘制出来。

