#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <string.h>


/* 定义数据包大小 */
#define PACKETSIZE 64

/* 定义数据包结构 */
struct packet
{
    struct icmphdr hdr; // ICMP头部
    char msg[PACKETSIZE-sizeof(struct icmphdr)]; // 数据负载
};

int pid=-1; // 进程ID
struct protoent *proto=NULL; // 协议实体
int cnt=1; // 计数器

/*--------------------------------------------------------------------*/
/*--- checksum - standard 1s complement checksum                   ---*/
/*--------------------------------------------------------------------*/
unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
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
int ping(char *adress)
{
    const int val=255;
    int i, sd;
    struct packet pckt;
    struct sockaddr_in r_addr;
    int loop;
    struct hostent *hname;
    struct sockaddr_in addr_ping,*addr;

    pid = getpid(); // 获取进程ID
    proto = getprotobyname("ICMP"); // 获取ICMP协议信息
    hname = gethostbyname(adress); // 获取主机信息
    bzero(&addr_ping, sizeof(addr_ping)); // 清零addr_ping结构
    addr_ping.sin_family = hname->h_addrtype;
    addr_ping.sin_port = 0;
    addr_ping.sin_addr.s_addr = *(long*)hname->h_addr;

    addr = &addr_ping; // 设置地址

    sd = socket(PF_INET, SOCK_RAW, proto->p_proto); // 创建原始套接字
    if ( sd < 0 )
    {
        perror("socket");
        return 1;
    }
    if ( setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0) // 设置TTL
    {
        perror("Set TTL option");
        return 1;
    }
    if ( fcntl(sd, F_SETFL, O_NONBLOCK) != 0 ) // 设置非阻塞I/O
    {
        perror("Request nonblocking I/O");
        return 1;
    }

    for (loop=0;loop < 10; loop++) // 循环发送和接收数据包
    {

        int len=sizeof(r_addr);

        if ( recvfrom(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &len) > 0 )
        {
            return 0; // 收到回应，返回0表示ping成功
        }

        bzero(&pckt, sizeof(pckt)); // 清零数据包结构
        pckt.hdr.type = ICMP_ECHO; // 设置ICMP回显
        pckt.hdr.un.echo.id = pid; // 设置进程ID
        for ( i = 0; i < sizeof(pckt.msg)-1; i++ )
            pckt.msg[i] = i +'0'; // 填充数据负载
        pckt.msg[i] = 0;  // 设置序列号

        pckt.hdr.un.echo.sequence = cnt++; // 设置序列号
        pckt.hdr.checksum = checksum(&pckt, sizeof(pckt)); // 计算并设置校验和
        if ( sendto(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)addr, sizeof(*addr)) <= 0 )
            perror("sendto"); // 发送数据包

        usleep(300000); // 等待300毫秒
    }

    return 1;
}

/*--------------------------------------------------------------------*/
/*--- main - look up host and start ping processes.                ---*/
/*--------------------------------------------------------------------*/
int main(int argc, char *argv[])
{

    if (ping("www.google.com"))
        printf("Ping is not OK. \n");
    else
        printf("Ping is OK. \n");


    return 0;
}