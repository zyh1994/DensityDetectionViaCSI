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

CSI信号，只有当WI-FI收发了数据后才会产生，为此我们需要准备一个通过WI-FI进行信号发送和接收的完整环境。目前我们采用的信号发送方法，是让PC对路由器发送PING，从而建立起完整的闭环。

> PING是一种用于测试网络连接的实用工具，它使用的是Internet控制消息协议（ICMP）。因此，PING既不是TCP协议，也不是UDP协议。ICMP位于IP层，是用于网络节点之间传递控制信息的协议。在PING操作中，主机会发送ICMP回显请求报文，目标主机会回复ICMP回显应答报文，从而确认网络连接是否正常。

所以建立CSI监听的过程大体上可以简化为，首先本机对其他设备发起PING（例如对路由器）

```bash
$(csi) sudo ping 192.168.0.1 -i 0.02
```

然后在本机使用tools/csi_listener.py脚本启动监听

```bash
$(csi) python3 csi_listener.py
```

启动后会首先列出设备中可用的网络设备名称，例如

* wlan01
* wifi01
* vpn01

从中选择Wi-Fi网卡

> 如果不知道自己的Wi-Fi网卡所对应的设备名，可以执行以下脚本进行确认
>
> ```bash
> $(csi) sh get_adapter_info.sh
> ```

选择好设备后，接下来会提示

* Log to file
* Transmit

选择 **Log to file** 会启动离线记录模式，会将WI-FI接收到的CSI信息直接以日志形式记录，并在当前文件夹下生成一个 csi.dat 的文件。该文件可以被 csikit 工具解析，也可以被 plotter/plotter_replayer.py 或者 Bfee 文件解析。

如果选择 **Transmit**，会启动一个TCP服务器，然后等待客户端接入。这个时候可以选择使用 plotter/plotter_online.py，它会连接上TCP服务器，然后绘制实时的数据，执行过程很简单。

```bash
$(csi) python plotter_online.py
```

# 数据结构说明

根据目前已有的资料，可以知道由Intel 5300网卡产生的CSI信息是长度不等的二进制数据帧。由于数据帧没有特定的包头和包尾，以及Hash验证，所以可以猜测该数据结构只能应用于一些环境相对简单的场景，并且不支持高并发的网络通信。所以如果需要应用在高并发场景，需要对原始数据帧进行解析后，再封装。

目前基本的数据帧如下：

```C
    struct iwl_bfee_notif
    {
        uint32_t /* __le32 */ timestamp_low;
        uint16_t /* __le16 */ bfee_count;
        uint16_t reserved1;
        uint8_t Nrx, Ntx;
        int8_t rssiA, rssiB, rssiC;
        int8_t noise;
        uint8_t agc, antenna_sel;
        uint16_t /* __le16 */ len;
        uint16_t /* __le16 */ fake_rate_n_flags;
        uint8_t payload[0];
    } __attribute__((packed));
```

数据帧的解析主要都在Bfee文件中，数据结构中包含了多个有关无线信号的参数，例如信号强度、噪声水平和天线配置等。数据结构的主要组成部分是一个名为Bfee的类，它包含了从原始字节数据中解析得到的无线信号的各种信息。

以下是Bfee数据结构的详细说明：

**timestamp_low**：表示测量CSI时的时间戳。
**bfee_count**：表示当前数据文件中当前CSI测量的编号。
**reserved1**：保留字段，未使用。
**nRx**：接收天线的数量。
**nTx**：发送天线的数量。
**rssi_a**、**rssi_b**、**rssi_c**：分别表示三个天线测量到的无线信号的接收信号强度指示器（RSSI）。
**noise**：无线信号的噪声水平。
**agc**：用于放大接收信号的自动增益控制（AGC）值。
**antenna_sel**：表示用于测量CSI的接收天线的二进制值。
**perm**：根据所选接收天线对CSI矩阵进行重新排序的排列矩阵。
**len**：包含CSI信息的有效载荷长度。
**fake_rate_n_flags**：包含有关CSI的各种标志和信息的字段。
**payload**：实际的CSI矩阵，描述了发送端和接收端之间的无线信道状态。这是一个30xNrxNtx的3D复数矩阵，其中30表示子载波的数量，Nrx和Ntx分别表示接收天线和发送天线的数量。

通过这个数据结构，我们可以对无线信道进行深入分析，从而改进无线通信系统的性能和可靠性。