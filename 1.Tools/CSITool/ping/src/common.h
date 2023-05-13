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

// c++ safe pointers
#include <memory> 


#ifndef PACKET_SIZE
#define PACKET_SIZE 64 // Size of the ping packet
#endif

// Create the raw socket
extern int create_raw_socket();

// Create the address structure with safe pointers
extern std::unique_ptr<struct sockaddr_in> create_address(const char* ip);

// Create the ICMP packet with safe pointers
extern std::unique_ptr<char[]> create_packet();

// Create the ICMP packet header with safe pointers
extern std::unique_ptr<struct icmphdr> create_packet_header(int seq, int pid);

// Send the Ping message
extern int send_ping(int sock_fd, std::unique_ptr<char[]>& packet, std::unique_ptr<struct sockaddr_in>& addr);

// Receive the Ping message
extern int recv_ping(int sock_fd, std::unique_ptr<char[]>& packet, std::unique_ptr<struct sockaddr_in>& addr);


#endif
