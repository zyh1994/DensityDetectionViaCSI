#include <signal.h>

#include "common.h"

#ifndef INFINITE_LOOP
#define INFINITE_LOOP true
#endif

// 计数器
int cnt=0;

// 协议实体
struct protoent *proto = NULL;

// Linux signal handler
volatile sig_atomic_t infinite_loop = 1;

void handle_signal(int signal)
{
    if (signal == SIGINT)
    {
        printf("Caught SIGINT signal, terminating...\n");
        infinite_loop = 0;
    }
}

/**
 * ping
*/
void ping(const char *address, useconds_t wait_time) {
    const int val = 255;
    struct packet pckt;
    struct sockaddr_in r_addr;
    int loop;

    struct sockaddr_in addr_ping;

    int const pid = getpid(); // 获取进程ID
    proto = getprotobyname("ICMP"); // 获取ICMP协议信息

    // struct hostent hname = gethostbyname(adress); // 获取主机信息
    // bzero(&addr_ping, sizeof(addr_ping)); // 清零addr_ping结构
    // addr_ping.sin_family = hname->h_addrtype;
    // addr_ping.sin_port = 0;
    // addr_ping.sin_addr.s_addr = *(long *) hname->h_addr;

    bzero(&addr_ping, sizeof(addr_ping)); // 清零addr_ping结构
    addr_ping.sin_family = AF_INET;
    addr_ping.sin_port = 0;
    inet_pton(AF_INET, address, &addr_ping.sin_addr);

    int sock_fd = socket(PF_INET, SOCK_RAW, proto->p_proto); // 创建原始套接字
    if (sock_fd < 0) {
        perror("socket");
        return;
    }
    if (setsockopt(sock_fd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0) // 设置TTL
    {
        perror("Set TTL option");
        return;
    }
    if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) != 0) // 设置非阻塞I/O
    {
        perror("Request nonblocking I/O");
        return;
    }


#if INFINITE_LOOP
    while (infinite_loop)
    {
#endif

        bzero(&pckt, sizeof(pckt)); // 清零数据包结构
        pckt.hdr.type = ICMP_ECHO; // 设置ICMP回显
        pckt.hdr.un.echo.id = pid; // 设置进程ID

        // filling the data payload
        int i = 0;
        for (i = 0; i < sizeof(pckt.msg) - 1; i++ ) {
            pckt.msg[i] = '0'; // 填充数据负载
        }
        pckt.msg[i] = 0;

        pckt.hdr.un.echo.sequence = ++cnt; // 设置序列号
        pckt.hdr.checksum = checksum(&pckt, sizeof(pckt)); // 计算并设置校验和

        if (send_ping(sock_fd, &addr_ping, &pckt) <= 0) {
            perror("sendto");
        }


        // wait several milliseconds
        usleep(wait_time);


        socklen_t len=sizeof(r_addr);

        if (recv_ping(sock_fd, &r_addr, &pckt) <= 0) {
            perror("recvfrom");
        } else {
            printf("%d bytes from %s: icmp_seq=%d ttl=%d\n",
                   PACKETSIZE, inet_ntoa(r_addr.sin_addr), cnt, val);
        }


#if INFINITE_LOOP
    }
#endif
}

/**
 * main
*/
int main(int argc, char *argv[])
{
    // check the input parameters
    // ping <address> <sample_rate>
    if (argc != 4 && argc != 3)
    {
        printf("Usage: ping <address> <sample_rate>\n");
        return 1;
    }
    // get the input parameters
    const char *address = argv[1];
    useconds_t sample_rate = 1000000 / static_cast<useconds_t>(atoi(argv[2]));

    // Register signal handler for SIGINT
    signal(SIGINT, handle_signal);

    // start ping
    ping(address, sample_rate);

    return 0;
}