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

#ifndef INFINITE_LOOP
#define INFINITE_LOOP true
#endif

// infinite loop flag
bool infinite_loop = false;

/* 定义数据包大小 */
#define PACKETSIZE 64

/* 定义数据包结构 */
struct packet
{
    struct icmphdr hdr; // ICMP头部
    char msg[PACKETSIZE - sizeof(struct icmphdr)]; // 数据负载
};

int pid=-1; // 进程ID
struct protoent *proto=NULL; // 协议实体
int cnt=1; // 计数器

/*--------------------------------------------------------------------*/
/*--- checksum - standard 1s complement checksum                   ---*/
/*--------------------------------------------------------------------*/
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


/*--------------------------------------------------------------------*/
/*--- ping - Create message and send it.                           ---*/
/*    return 0 is ping Ok, return 1 is ping not OK.                ---*/
/*--------------------------------------------------------------------*/
void ping(char *adress, useconds_t wait_time) {
    const int val = 255;
    struct packet pckt;
    struct sockaddr_in r_addr;
    int loop;
    struct hostent *hname;
//    struct sockaddr_in addr_ping,*addr;
    struct sockaddr_in addr_ping;

    pid = getpid(); // 获取进程ID
    proto = getprotobyname("ICMP"); // 获取ICMP协议信息
    hname = gethostbyname(adress); // 获取主机信息
    bzero(&addr_ping, sizeof(addr_ping)); // 清零addr_ping结构
    addr_ping.sin_family = hname->h_addrtype;
    addr_ping.sin_port = 0;
    addr_ping.sin_addr.s_addr = *(long *) hname->h_addr;

//    addr = &addr_ping; // 设置地址

    int sock_fd = socket(PF_INET, SOCK_RAW, proto->p_proto); // 创建原始套接字
    if (sock_fd < 0) {
        perror("socket");
//        return 1;
        return;
    }
    if (setsockopt(sock_fd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0) // 设置TTL
    {
        perror("Set TTL option");
//        return 1;
        return;
    }
    if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) != 0) // 设置非阻塞I/O
    {
        perror("Request nonblocking I/O");
//        return 1;
        return;
    }

#if INFINITE_LOOP
    while (infinite_loop)
    {
#endif
        socklen_t len=sizeof(r_addr);

        // receive message from remote
        if ( recvfrom(sock_fd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &len) <= 0 )
        {
            // Print out the error message if no message received
            perror("recvfrom");
        } 
        else { // Print out the message if received
            printf("%d bytes from %s: icmp_seq=%d ttl=%d\n",
                   PACKETSIZE, inet_ntoa(r_addr.sin_addr), cnt, val);
        }

        bzero(&pckt, sizeof(pckt)); // 清零数据包结构
        pckt.hdr.type = ICMP_ECHO; // 设置ICMP回显
        pckt.hdr.un.echo.id = pid; // 设置进程ID

        // filling the data payload
        int i = 0;
        for (i = 0; i < sizeof(pckt.msg) - 1; i++ ) {
            pckt.msg[i] = '0'; // 填充数据负载
        }
        pckt.msg[i] = 0;  // 设置序列号

        pckt.hdr.un.echo.sequence = cnt++; // 设置序列号
        pckt.hdr.checksum = checksum(&pckt, sizeof(pckt)); // 计算并设置校验和

        // send data to remote device
        if ( sendto(sock_fd, &pckt, sizeof(pckt), 0,
                    (struct sockaddr*)&addr_ping, sizeof(addr_ping)) <= 0 )
        {
            perror("sendto"); // 发送数据包
        }

        // wait several milliseconds
        usleep(wait_time);
#if INFINITE_LOOP
    }
#endif
}

/*--------------------------------------------------------------------*/
/*--- main - look up host and start ping processes.                ---*/
/*--------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    // check the input parameters
    // ping <address> <wait_time> <infinite_loop> or ping <address> <wait_time>
    if (argc != 4 && argc != 3)
    {
        printf("Usage: ping <address> <wait_time> <infinite_loop>\n");
        return 1;
    }

    // get the input parameters
    char *address = argv[1];
    useconds_t wait_time = static_cast<useconds_t>(atoi(argv[2]));

    if (argc == 4)
    {
        infinite_loop = static_cast<bool>(atoi(argv[3]));
    }

    // start ping
    ping(address, wait_time);

    return 0;
}