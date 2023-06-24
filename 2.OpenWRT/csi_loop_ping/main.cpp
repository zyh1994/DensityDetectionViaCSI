#include <arpa/inet.h>              // 支持网络编程的函数和常量
#include <linux/if_packet.h>        // 与网络设备相关的数据类型和函数
#include <stdio.h>                  // 标准输入/输出函数和数据类型
#include <string.h>                 // 字符串操作函数
#include <stdlib.h>                 // 通用函数和常量
#include <sys/ioctl.h>              // 对设备进行操作的函数
#include <sys/socket.h>             // 套接字接口
#include <sys/types.h>              // 基本的系统数据类型
#include <sys/stat.h>               // 文件状态函数和数据类型
#include <fcntl.h>                  // 文件控制函数
#include <termios.h>                // 终端I/O接口
#include <net/if.h>                 // 与网络接口相关的数据类型和函数
#include <netinet/ether.h>          // 以太网函数和数据类型
#include <unistd.h>                 // 对POSIX操作系统API的访问
#include <errno.h>                  // 错误号
#include <signal.h>                 // 信号处理函数

#include <chrono>                   // 时间函数

/* 定义默认的目标MAC地址 */
#define DEFAULT_DEST_MAC0	0x00
#define DEFAULT_DEST_MAC1	0x03
#define DEFAULT_DEST_MAC2	0x7F
#define DEFAULT_DEST_MAC3	0xB0
#define DEFAULT_DEST_MAC4	0x20
#define DEFAULT_DEST_MAC5	0x20

#define DEFAULT_IF	        "wlan0"     // 默认的网络接口名
#define BUF_SIZ	            2048        // 缓冲区大小

int sockfd;                             // 套接字文件描述符
bool quit;                              // 退出标志


/* 处理SIGINT信号 */
void handle_sigint(int sig)
{
    if (sig == SIGINT) {
        /* 打印信号值 */
        printf("Caught signal %d\n", sig);

        /* 设置退出标志 */
        quit = true;
    }
}


int main(int argc, char *argv[])
{
    int     i;                       // 循环变量
    struct  ifreq if_idx;            // 网络接口请求结构体
    struct  ifreq if_mac;            // 网络接口MAC地址
    int     tx_len = 0, cnt;         // 发送的数据长度和包的数量
    char    sendbuf[BUF_SIZ];        // 发送数据的缓冲区
    unsigned int DstAddr[6];         // 目标MAC地址
    struct  ether_header *eh = (struct ether_header *) sendbuf;  // 以太网头部 (未使用)
    struct  iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));  // IP头部（未使用）
    struct  sockaddr_ll socket_address; // 套接字地址结构体
    char    ifName[IFNAMSIZ];        // 网络接口名

    /* 注册信号处理函数 */
    signal(SIGINT, handle_sigint);

    /* 检查命令行参数并设置接口名、目标MAC地址和包的数量 */
    if (argc == 1) {
        printf("Usage: %s ifName DstMacAddr NumOfPacketToSend\n", argv[0]);
        printf("Example: %s wlan0 00:7F:5D:3E:4A 100\n", argv[0]);
        exit(0);
    }

    /* 获取接口名 */
    if (argc > 1)
        strcpy(ifName, argv[1]);
    else
        strcpy(ifName, DEFAULT_IF);

    /* 目标地址用:分隔，例如：00:7F:5D:3E:4A */
    if(argc > 2) {
        sscanf(argv[2],"%x:%x:%x:%x:%x:%x",
                &DstAddr[0], &DstAddr[1], &DstAddr[2], &DstAddr[3], &DstAddr[4], &DstAddr[5]);
    }
    else {
        DstAddr[0] = DEFAULT_DEST_MAC0;
        DstAddr[1] = DEFAULT_DEST_MAC1;
        DstAddr[2] = DEFAULT_DEST_MAC2;
        DstAddr[3] = DEFAULT_DEST_MAC3;
        DstAddr[4] = DEFAULT_DEST_MAC4;
        DstAddr[5] = DEFAULT_DEST_MAC5;
    }

    if(argc > 3)
        cnt = atoi(argv[3]);
    else
        cnt = 1;

    /* 计算发送包的间隔时间 */
    int usecs = 1000000 / cnt;

    /* 打开一个RAW套接字以进行发送 */
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        perror("socket");
    }

    /* 获取接口的索引 */
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
        perror("SIOCGIFINDEX");

    /* 获取接口的MAC地址 */
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
        perror("SIOCGIFHWADDR");

    /* 构造以太网头部 */
    memset(sendbuf, 0, BUF_SIZ);

    /* 以太网头部 */
    eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
    eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
    eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
    eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
    eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
    eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
    eh->ether_dhost[0] = DstAddr[0];
    eh->ether_dhost[1] = DstAddr[1];
    eh->ether_dhost[2] = DstAddr[2];
    eh->ether_dhost[3] = DstAddr[3];
    eh->ether_dhost[4] = DstAddr[4];
    eh->ether_dhost[5] = DstAddr[5];

    /* 以太网类型 */
    eh->ether_type = htons(ETH_P_IP);
    tx_len += sizeof(struct ether_header);

    /* 设置数据包的数据部分 */
    memset(sendbuf + tx_len, 0xaa, BUF_SIZ - tx_len);
    printf("Packet Length is: %d, packet number is: %d\n", tx_len, cnt);

    /* 设置套接字地址结构体 */
    socket_address.sll_ifindex = if_idx.ifr_ifindex;    // 接口索引
    socket_address.sll_family = AF_PACKET;              // 套接字类型
    socket_address.sll_protocol=htons(ETH_P_IP);        // 以太网类型
    socket_address.sll_hatype = ARPHRD_ETHER;           // 帧类型
    socket_address.sll_pkttype = PACKET_OTHERHOST;      // 包类型
    socket_address.sll_halen = ETH_ALEN;                // MAC地址长度

    /* 设置目标MAC地址 */
    memcpy(socket_address.sll_addr, DstAddr, 6);

    /* Set the socket as non-blocking */
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("Failed to get socket flags");
        return 1;
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("Failed to set socket as non-blocking");
        return 1;
    }

    /* 计数器 */
    int count = 0;

    /* 计时器 */
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    /* 设置退出标志 */
    quit = false;

    /* 时间字符串 */
    char time_str[100];

    /* 发送数据包 */
    while(!quit) {

        /* 按着间隔时间发送数据包 */
        usleep(usecs);

        /* 发送数据包 */
        if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) == -1) {
            printf("Send failed: %s\n", strerror(errno));
        }

        /* 计数器加1 */
        count++;

        /* 获取当前时间 */
        end = std::chrono::system_clock::now();

        /* 每10s打印一次发送的数据包数量和当前时间 */
        if (std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= 10) {
            std::time_t end_time = std::chrono::system_clock::to_time_t(end);
            std::tm* local_time = std::localtime(&end_time);
            std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

            printf("%s - Send %d packages\n", time_str, count);
            count = 0;
            start = std::chrono::system_clock::now();
        }
    }

    /* 关闭套接字 */
    close(sockfd);

    /* 打印退出信息 */
    printf("Exit the program\n");

    /* 退出 */
    return 0;
}
