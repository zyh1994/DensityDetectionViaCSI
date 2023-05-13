#include "common.h"


// Calculate the Checksum
unsigned short calculate_checksum(unsigned short *addr, int len) {
    unsigned long sum = 0;
    while (len > 1) {
        sum += *addr++;
        len -= 2;
    }
    if (len == 1) {
        sum += *(unsigned char *)addr;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}


// Create the raw socket
int create_raw_socket() 
{
    int sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int ttl = 64;
    int enable = 1;

    if (sock_fd < 0) {
        perror("socket");
        return -1;
    }

    // Set the socket as non-blocking
    if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl");
        return -1;
    }
    // Set the TTL
    if (setsockopt(sock_fd, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("setsockopt");
        return -1;
    }
    // Set the socket as reusable
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt");
        return -1;
    }

    return sock_fd;
};


// Create the address structure with safe pointers
std::unique_ptr<struct sockaddr_in> create_address(const char* ip) 
{
    auto addr = std::make_unique<struct sockaddr_in>();

    /* Set the address */
    bzero(addr.get(), sizeof(*addr)); // Clear the address structure
    addr->sin_family = AF_INET; // Set address family to Internet
    addr->sin_port = 0; // Set port to 0
    
    if (inet_pton(AF_INET, ip, &(addr->sin_addr)) <= 0) {  // Set the IP address to ping
        perror("inet_pton error");
        return nullptr;
    }

    return addr;
};


// Create the ICMP packet header with safe pointers
std::unique_ptr<struct icmphdr> create_packet_header(int seq, int pid) 
{
    auto packet_hdr = std::make_unique<struct icmphdr>();

    /* ICMP Header */
    packet_hdr->type = ICMP_ECHO; // Message type
    packet_hdr->code = 0; // Message code
    packet_hdr->un.echo.id = pid; // Process ID
    packet_hdr->un.echo.sequence = seq; // Sequence number
    packet_hdr->checksum = 0; // Checksum
    packet_hdr->checksum = calculate_checksum((unsigned short*)packet_hdr.get(), PACKET_SIZE); // Calculate the checksum

    return packet_hdr;
};


// Create the ICMP packet with safe pointers
std::unique_ptr<char[]> create_packet() 
{
    /* Create the packet */
    auto packet = std::make_unique<char[]>(PACKET_SIZE);
    memset(packet.get(), 0, PACKET_SIZE); // Clear the packet

    /* Copy the packet header to the packet */
    auto packet_hdr = create_packet_header(0, 0); // Create the packet header
    memcpy(packet.get(), packet_hdr.get(), sizeof(*packet_hdr)); // Copy the packet header to the packet

    return packet;
};


// Send the Ping message
int send_ping(int sock_fd, std::unique_ptr<char[]>& packet, std::unique_ptr<struct sockaddr_in>& addr)
{
    if (sendto(sock_fd, packet.get(), PACKET_SIZE, 0, (struct sockaddr*)addr.get(), sizeof(*addr)) <= 0) {
        perror("sendto error");
        return -1;
    }

    return 0;
};


// Receive the Ping message
int recv_ping(int sock_fd, std::unique_ptr<char[]>& packet, std::unique_ptr<struct sockaddr_in>& addr)
{
    if (recvfrom(sock_fd, packet.get(), PACKET_SIZE, 0, (struct sockaddr*)addr.get(), (socklen_t*)sizeof(*addr)) <= 0) {
        perror("recvfrom error");
        return -1;
    }

    return 0;
};
