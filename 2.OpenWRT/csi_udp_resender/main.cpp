#include <stdlib.h>             // 通用函数和常量
#include <stdio.h>              // 标准输入/输出函数和数据类型
#include <string.h>             // 字符串操作函数
#include <unistd.h>             // 对POSIX操作系统API的访问
#include <fcntl.h>              // 文件控制函数
#include <errno.h>              // 错误号
#include <termios.h>            // 终端I/O接口
#include <pthread.h>            // 线程相关函数
#include <signal.h>             // 信号处理函数
#include <sys/types.h>          // 基本的系统数据类型
#include <sys/stat.h>           // 文件状态函数和数据类型

#include <chrono>               // 时间函数

#include <sys/socket.h>         // socket函数和相关常量
#include <netinet/in.h>         // sockaddr_in结构体和相关函数
#include <arpa/inet.h>          // inet_addr函数

#include "csi_fun.h"

#define BUFSIZE 4096

int quit;

/**
 * @brief Handle the Ctrl+C signal
 * @param sig
 */
void handle_sigint(int sig)
{
    if (sig == SIGINT) {
       
        /* print the signal number */
        printf("Caught signal %d\n", sig);

        /* set the quit flag */
        quit = true;
    }
}

/**
 * @brief Setup the UDP broadcast socket
 * @param socket_fd
 * @param server_addr
 * @param ip_addr
 * @param port
 */
void setup_udp_broadcast(int *socket_fd, struct sockaddr_in *server_addr, const char *ip_addr, const int port) {
    int broadcast = 1;

    /* Create socket */
    if ((*socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    /* Set the socket as non-blocking */
    int flags = fcntl(*socket_fd, F_GETFL, 0);
    if (flags == -1) {
        perror("Failed to get socket flags");
        exit(1);
    }

    if (fcntl(*socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("Failed to set socket as non-blocking");
        exit(1);
    }

    /* Allow broadcast */
    if (setsockopt(*socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    /* Bind to the port */  
    bzero(server_addr, sizeof(struct sockaddr_in));

    /* Set family and port */
    server_addr->sin_family = AF_INET;

    /* Convert port number to integer */
    server_addr->sin_port = htons(port);

    /* Convert IP address to integer */
    if (inet_pton(AF_INET, ip_addr, &(server_addr->sin_addr)) <= 0) {
        perror("Invalid IP address.");
        exit(1);
    }
}

/**
 * @brief Broadcast the data to the client
 * @param socket_fd
 * @param client_addr
 * @param data
 * @param len
 */
void broadcast_udp(int socket_fd, struct sockaddr_in *client_addr, unsigned char *data, int len) {

    /* Send data via UDP broadcast */
    if (sendto(socket_fd, data, len, 0, (struct sockaddr *)client_addr, sizeof(struct sockaddr)) < 0) {
        perror("sendto");
        exit(1);
    }
}

/**
 * @brief Main function
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
    /* CSI device file descriptor */
    int             csi_fd;        

    /* Register the signal handler to handle Ctrl+C */
    signal(SIGINT, handle_sigint);

    /* Print the usage when the number of parameters is not correct */
    if (argc != 3) {
        printf("Usage: %s <BroadCast IP> <Port>\n", argv[0]);
        printf("-- Example: %s 192.168.1.255 8888\n", argv[0]);
        return 0;
    }

    /* Open the device file */
    csi_fd = open_csi_device();
    if (csi_fd < 0) {
        perror("Failed to open the device...");
        return errno;
    }

    /* UDP socket file descriptor */
    int socket_fd;

    /* UDP server address */
    struct sockaddr_in broadcast_addr;

    /* Get the IP address and port number */
    const char *ip_addr = argv[1];
    const int port = atoi(argv[2]);

    /* Setup the UDP broadcast socket */
    setup_udp_broadcast(&socket_fd, &broadcast_addr, ip_addr, port);

    /* Prepare the buffer to store the data */
    unsigned char buf_addr[BUFSIZE] = {0};

    /* data length */
    int cnt = 0;

    /* quit flag */
    quit = false;

    /* timer */
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    /* counter */
    int count = 0;

    /* time string */
    char time_str[100];

    /* Main loop */
    while(!quit) {

        /* Read the CSI data from the device file */
        cnt = read_csi_buf(buf_addr, csi_fd, BUFSIZE);

        /* If there is data, broadcast it */
        if (cnt > 0) {

            /* Broadcast the data */
            broadcast_udp(socket_fd, &broadcast_addr, buf_addr, cnt);
                   
            /* Increase the counter */
            count++;

            /* Get the current time */
            end = std::chrono::system_clock::now();

            /* If 10 seconds passed, print the number of packages */
            if (std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= 10) {
                std::time_t end_time = std::chrono::system_clock::to_time_t(end);
                std::tm* local_time = std::localtime(&end_time);
                std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

                printf("%s - Received %d packages\n", time_str, count);            
                count = 0;
                start = std::chrono::system_clock::now();
            }
        }
    }

    /* Close the device file */
    close_csi_device(csi_fd);

    /* Close the UDP socket */
    return 0;
}