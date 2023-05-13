#ifndef __PING_H__
#define __PING_H__

#include <fcntl.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <cstdlib>

#ifndef PACKETSIZE
/* 定义数据包大小 */
#define PACKETSIZE 64
#endif

/* 定义数据包结构 */
struct packet
{
    struct icmphdr hdr; // ICMP头部
    char msg[PACKETSIZE - sizeof(struct icmphdr)]; // 数据负载
};

struct ping_dev_info
{
    int sock_fd;
    struct sockaddr_in addr;
    struct packet packet; // ICMP packe
};


// Calculate the checksum of a packet
extern unsigned short checksum(void *b, int len);

// Send the Ping message
extern int send_ping(int sock_fd, struct sockaddr_in* addr, struct packet* pack);

// Send the Ping message
extern int send_ping(struct ping_dev_info* dev_info);

// Receive the feedback message
extern int recv_ping(int sock_fd, struct sockaddr_in* addr, struct packet* pack);

// Receive the feedback
extern int recv_ping(struct ping_dev_info* dev_info);

// Create the ICMP socket
extern int create_icmp_sock();

// Set the ICMP address struct
extern void set_icmp_addr_struct(struct sockaddr_in* addr, const char *address);

// Set the ICMP header
extern void set_icmp_header(struct packet* pack, int seq, int pid);

#endif