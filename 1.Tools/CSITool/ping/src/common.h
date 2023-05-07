#ifndef __PING_H__
#define __PING_H__

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

// Calculate the checksum of a packet
extern unsigned short checksum(void *b, int len);

// Send the Ping message
extern int send_ping(int sock_fd, struct sockaddr_in* addr, struct packet* pack);

// Receive the Ping message
extern int recv_ping(int sock_fd, struct sockaddr_in* addr, struct packet* pack);

#endif