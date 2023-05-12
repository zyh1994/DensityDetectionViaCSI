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
