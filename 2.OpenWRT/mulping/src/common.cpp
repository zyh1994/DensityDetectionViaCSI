#include "common.h"

// Calculate the Checksum
unsigned short checksum(void *b, int len)
{
    auto *buf = static_cast<unsigned short *>(b);
    unsigned int sum;
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
    bzero(pack, sizeof(*pack)); // 清零数据包结构
    pack->hdr.type = ICMP_ECHO; // 设置ICMP回显
    pack->hdr.un.echo.id = pid; // 设置进程ID

    // filling the data payload
    int i;
    for (i = 0; i < sizeof(pack->msg) - 1; i++ ) {
        pack->msg[i] = '0'; // 填充数据负
    }
    pack->msg[i] = 0;  // 设置序列号

    pack->hdr.un.echo.sequence = seq; // 设置序列号
    pack->hdr.checksum = checksum(pack, sizeof(*pack)); // 计算并设置校验和
}


// Send the Ping message
int send_ping(int sock_fd, struct sockaddr_in* addr, struct packet* pack)
{
    socklen_t len = sizeof(*addr);
    return sendto(sock_fd, pack, sizeof(*pack), 0, (struct sockaddr*)addr, len);
}


// Send the ping message
int send_ping(struct ping_dev_info* dev_info)
{
    return send_ping(dev_info->sock_fd, &(dev_info->addr), &(dev_info->packet));
}


// Receive the feedback message
int recv_ping(int sock_fd, struct sockaddr_in* addr, struct packet* pack)
{
    socklen_t len = sizeof(*addr);
    return recvfrom(sock_fd, pack, sizeof(*pack), 0, (struct sockaddr*)addr, &len);
}


// Receive the feedback message
int recv_ping(struct ping_dev_info* dev_info)
{
    return recv_ping(dev_info->sock_fd, &(dev_info->addr), &(dev_info->packet));
}


// Create the ICMP socket
int create_icmp_sock()
{
    // 获取ICMP协议信息
    struct protoent *proto = getprotobyname("ICMP");

    // Create TTL
    int val = 255;

    int sock_fd = socket(PF_INET, SOCK_RAW, proto->p_proto); // 创建原始套接字
    if (sock_fd < 0) {
        perror("socket");
        return -1;
    }
    if (setsockopt(sock_fd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0) // 设置TTL
    {
        perror("Set TTL option");
        return -1;
    }
   if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) != 0) // 设置非阻塞I/O
   {
       perror("Request nonblocking I/O");
       return -1;
   }

    return sock_fd;
}


void set_icmp_addr_struct(struct sockaddr_in* addr, const char *address)
{
    bzero(addr, sizeof(*addr)); // 清零addr_ping结构
    addr->sin_family = AF_INET;
    addr->sin_port = 0;
    inet_pton(AF_INET, address, &(addr->sin_addr));
}