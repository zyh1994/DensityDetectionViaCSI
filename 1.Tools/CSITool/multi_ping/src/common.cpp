#include "common.h"

// Calculate the Checksum
unsigned short checksum(void *b, int len)
{
    auto *buf = static_cast<unsigned short *>(b);
    unsigned int sum=0;
    unsigned short result;

    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}


void set_icmp_header(struct packet* pack, int seq, int pid)
{
    hdr->type = ICMP_ECHO; // 设置ICMP回显
    hdr->un.echo.id = pid; // 设置进程ID

    // filling the data payload
    int i = 0;
    for (i = 0; i < sizeof(hdr->un.echo.data) - 1; i++ ) {
        hdr->un.echo.data[i] = '0'; // 填充数据负载
    }
    hdr->un.echo.data[i] = 0;

    hdr->un.echo.sequence = seq; // 设置序列号
    hdr->checksum = checksum(hdr, sizeof(*hdr)); // 计算并设置校验和


    bzero(pack, sizeof(*pack)); // 清零数据包结构
    pack->hdr.type = ICMP_ECHO; // 设置ICMP回显
    pack->hdr.un.echo.id = pid; // 设置进程ID

    // filling the data payload
    int i = 0;
    for (i = 0; i < sizeof(pack->msg) - 1; i++ ) {
        pack->msg[i] = '0'; // 填充数据负载
    }
    pack->msg[i] = 0;

    pack->hdr.un.echo.sequence = seq; // 设置序列号
    pack->hdr.checksum = checksum(pack, sizeof(*pack)); // 计算并设置校验和
}


// Send the Ping message
int send_ping(int sock_fd, struct sockaddr_in* addr, struct packet* pack) 
{
    socklen_t len = sizeof(*addr);
    return sendto(sock_fd, pack, sizeof(*pack), 0, (struct sockaddr*)addr, len);
}


// Receive the Ping message
int recv_ping(int sock_fd, struct sockaddr_in* addr, struct packet* pack)
{
    socklen_t len = sizeof(*addr);
    return recvfrom(sock_fd, pack, sizeof(*pack), 0, (struct sockaddr*)addr, &len);
}