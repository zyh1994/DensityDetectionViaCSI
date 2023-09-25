**Table Of Content**

- [Install the Docker Image for ESP32 CSI project \[2023/09/25\]](#install-the-docker-image-for-esp32-csi-project-20230925)
  - [Install Docker](#install-docker)
    - [Install Docker on Ubuntu](#install-docker-on-ubuntu)
    - [Install Docker on Windows](#install-docker-on-windows)
  - [Install the Docker Image](#install-the-docker-image)
  - [Compile the ESP32 CSI project](#compile-the-esp32-csi-project)
    - [Use the Docker Image to Compile the ESP32 CSI project](#use-the-docker-image-to-compile-the-esp32-csi-project)
    - [Flash the ESP32 CSI project](#flash-the-esp32-csi-project)
    - [Monitor the ESP32 CSI project](#monitor-the-esp32-csi-project)
    - [Clean the ESP32 CSI project](#clean-the-esp32-csi-project)




# Install the Docker Image for ESP32 CSI project [2023/09/25]

## Install Docker

### Install Docker on Ubuntu

```bash
sudo apt-get update
sudo apt-get install docker.io
```

### Install Docker on Windows

Download the Docker Desktop for Windows from [here](https://hub.docker.com/editions/community/docker-ce-desktop-windows/).


When downloaded docker-desktop-installer.exe, double click it to install. It will ask you to choose between WSL2 and Virtual Machine. From my experience, WSL2 is better than Hyper-V. So, I recommend you to choose WSL2.


## Install the Docker Image

Open the terminal (if you are using Windows, open the PowerShell) and type the following command.

```bash
bash
```

Then, type the following command.

```bash
docker pull espressif/idf:v5.0.4
```

to download the official Docker image for ESP32 development. As ESP32S3 is used in this project, the version of the Docker image is v5.0.4. For this project, we used the Docker image v5.0.4, and it worked well. However, if you want to use the latest version of the Docker image, you can use it. But, I cannot guarantee that it will work well.

## Compile the ESP32 CSI project

We have the following resources can be used for the ESP32 project.


- [ESP32 IDF](https://github.com/espressif/esp-idf)
- [ESP32 CSI](https://github.com/espressif/esp-csi)
- [Docker Image for ESP32 Development](https://hub.docker.com/r/espressif/idf)

The ESP32 IDF is the official development framework for ESP32. The ESP32 CSI is the official CSI library for ESP32. The ESP32 CSI library is based on the ESP32 IDF. So, we need to install the ESP32 IDF first, and then install the ESP32 CSI library.

However, when installing the ESP32 IDF, we may encounter some problems. That is why we use the Docker image for ESP32 related projects to avoid the problems. 

However, if you want to install the ESP32 IDF without using the Docker image, you can follow the instructions from [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html).

### Use the Docker Image to Compile the ESP32 CSI project

Suppose that you have already pulled the Docker image for ESP32 development. Then, you can use the following command to compile the ESP32 CSI project.

```bash
docker run -it --rm -v $PWD:/project -w /project espressif/idf:v5.0.4 idf.py build
```

* `docker run` is used to run the Docker image.
* `-it` is used to run the Docker image in the interactive mode.
* `--rm` is used to remove the Docker container after the Docker image is executed.
* `-v $PWD:/project` is used to mount the current directory to the `/project` directory in the Docker container.
* `-w /project` is used to set the working directory to the `/project` directory in the Docker container.
* `espressif/idf:v5.0.4` is the name of the Docker image.
* `idf.py build` is the command to compile the ESP32 CSI project.

For example, if you want to compile one of the examples in the ESP32 project, like `hello_world`, you can use the following command.

```bash
cd examples/get-started/hello_world
```

`CD` to the `hello_world` directory. Then, use the following command to compile the `hello_world` example.


```bash
docker run -it --rm -v $PWD:/project -w /project espressif/idf:v5.0.4 idf.py build
```

### Flash the ESP32 CSI project


After the compilation is finished, you can find the `hello_world.bin` file in the `build` directory. Then, you can use the following command to flash the `hello_world.bin` file to the ESP32S3 board.

```bash
docker run -it --rm -v $PWD:/project -w /project espressif/idf:v5.0.4 idf.py flash
```

### Monitor the ESP32 CSI project

After the flashing is finished, you can use the following command to monitor the ESP32S3 board.

```bash
docker run -it --rm -v $PWD:/project -w /project espressif/idf:v5.0.4 idf.py monitor
```

### Clean the ESP32 CSI project

If you want to clean the ESP32 CSI project, you can use the following command.

```bash
docker run -it --rm -v $PWD:/project -w /project espressif/idf:v5.0.4 idf.py fullclean
```
