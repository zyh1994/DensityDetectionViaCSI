# 程序使用方式说明

## CSI 链路准备

首先，需要准备发送和接收用的OpenWRT设备。分别在充当发送和接收端的设备上编译安装 csi_loop_ping 和 csi_udp_resender （在当前的测试环境中已经安装）。

首先，请在充当信号接收端的OpenWRT设备上执行如下命令：

```bash
ifconfig
```

然后会看到这样一行信息，你需要记录下这里wlan0端口对应的MAC地址

```
wlan0	Link encap:Ethernet	HWaddr 00:7F:5D:3E:4A
	inet addr:192.168.4.1  Bcast:192.168.4.255  Mask 255.255.255.0
```

然后，在发送端的OpenWRT设备上，执行如下命令：

```bash
csi_loop_ping wlan0 00:7F:5D:3E:4A <sampling_rate>
```

sampling_rate 是采样率，指的是一秒钟发送信号的次数，默认最低为1次，即1秒钟发送1次信号。如果需要一秒钟发送多次信号，例如20次信号

```bash
csi_loop_ping wlan0 00:7F:5D:3E:4A 20
```

这里推荐 sampling rate 为20。当信号开始发送后，请回到接收端的 OpenWRT 设备上，启动数据转发程序，为了确保数据传输质量，该路由器最好是通过有线网络与负责接收和信号处理的PC机直连。

由于接收端 OpenWRT 设备是被动监听 CSI 请求，主动 UDP 广播转发的模式，所以需要指定转发端口，以及广播地址。由于在我们的试验环境中，是通过两个网段分别建立与接收端的 CSI 数据通路，与 PC 机的信号处理通路。因此在进行信号广播时，就需要使用与 PC 相同网段的广播地址，例如本例中

```bash
csi_udp_resender 192.168.2.255 8080
```

其中 8080 是端口号。至此，CSI 信号发送与转发链路如果不出意外的情况下，就算建立起来了。


## PC 端信号接收

### 编译

因为本项目是 C/C++ 工程，所以在PC接收端编译该工程。工程使用CMake，所以请先在 Console 下，当前的目录下创建一个名为 build 的文件夹。

```bash
$ mkdir build
```

然后 cd 到 build

```bash
$ cd build
```

然后执行如下命令

```bash
build$ cmake ..
```

CMake 如果顺利，没有报错的话，就可以直接执行 make 命令

```bash
build$ make
```

然后可以在 build 下生成可执行程序 `pc_sampling`。

### 使用

假设已经编译出来 `pc_sampling`, 要让它成功启动需要传入以下三个参数

* port 端口号
* width 生成的视频的 width
* height 生成的视频的 height

我们可以使用 320x240 作为视频大小。

```bash
./pc_sampling 8080 320 240
```

程序启动后，会打开摄像头，并开始把接收到的 CSI 信号同步写入到一个 .dat 的文件中，该文件每1分钟生成1个。

## BIN文件的解析

### 解析BIN文件

在本工程的 `tools` 目录下，有另外两个 Python 工程。分别是 BIN 文件，以及当 BIN 文件解析完毕后，回放视频和CSI信号。

解析BIN的话，请首先 cd 到

```bash
$ cd tools/BinParser
```

然后执行以下命令

```bash
tools/BinParser$ python3 parser.py xxxx.bin
```

其中 `xxxx.bin` 是本工程生成的bin文件，解析完BIN文件后，会在BIN文件同一目录中，生成解析后的图片、视频以及CSI的数据，并存储到名为 `xxxx` 文件夹。


### 回放CSI信号

首先，请cd到信号回放工程

```bash
$cd tools/CSVDataReplay
```

然后执行以下命令

```bash
tools/CSVDataReplay$ python3 replayer.py xxxx
```
其中，`xxxx` 是由上面的工程生成的文件夹地址，例如 对于 `csi_202307021020.bin` 文件，会在同一文件夹下生成一个名为 `csi_202307021020` 的文件夹，然后

```bash
tools/CSVDataReplay$ python3 replayer.py path/to/csi_202307021020
```

此外该工程还可以输入另外两个参数

```bash
[folder_path] [start_frame] [end_frame]
```

`start_frame` 和  `end_frame` 是指的回放的开始帧和结束帧的位置。