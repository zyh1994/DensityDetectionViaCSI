#!/bin/bash

# 下载CUDA 10.1安装程序
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1604/x86_64/cuda-ubuntu1604.pin || { echo "Error: failed to download CUDA installation file"; exit 1; }
wget https://developer.download.nvidia.com/compute/cuda/10.1/Prod/local_installers/cuda_10.1.243_418.87.00_linux.run || { echo "Error: failed to download CUDA installation file"; exit 1; }

# 复制cuda-ubuntu1604.pin文件到/etc/apt/preferences.d/目录中
sudo mv cuda-ubuntu1604.pin /etc/apt/preferences.d/cuda-repository-pin-600 || { echo "Error: failed to move CUDA installation file"; exit 1; }

# 添加CUDA软件源
sudo sh -c 'echo "deb http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1604/x86_64 /" > /etc/apt/sources.list.d/cuda.list' || { echo "Error: failed to add CUDA repository"; exit 1; }

# 更新软件包索引并安装CUDA Toolkit和驱动程序
sudo apt-get update || { echo "Error: failed to update package index"; exit 1; }
sudo apt-get -y install cuda --allow-unauthenticated || { echo "Error: failed to install CUDA Toolkit"; exit 1; }

# 设置NVIDIA驱动程序
sudo apt-get -y install nvidia-driver-450 || { echo "Error: failed to install NVIDIA driver"; exit 1; }

# 验证CUDA是否安装成功
if ! command -v nvcc &> /dev/null
then
    echo "Error: CUDA installation failed"
    exit 1
else
    echo "CUDA has been successfully installed"
fi

# 将CUDA安装目录添加到PATH和LD_LIBRARY_PATH环境变量中
echo 'export PATH=/usr/local/cuda-10.1/bin${PATH:+:${PATH}}' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=/usr/local/cuda-10.1/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}' >> ~/.bashrc

# 重新加载.bashrc文件
source ~/.bashrc
