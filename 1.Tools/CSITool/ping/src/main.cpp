#include <signal.h>

#include "common.h"

#ifndef INFINITE_LOOP
#define INFINITE_LOOP true
#endif

// 计数器
int cnt=0;

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

    // Create the raw socket
    int sock_fd = create_raw_socket();

    // Create the address structure with safe pointers
    auto addr = create_address(address);

    // Create the ICMP packet with safe pointers
    auto packet = create_packet();


#if INFINITE_LOOP
    while (infinite_loop)
    {
#endif

        // Create the ICMP packet header with safe pointers
        auto packet_header = create_packet_header(cnt, getpid());

        // Copy the packet header to the packet
        memcpy(packet.get(), packet_header.get(), sizeof(struct icmphdr));

        // Send the Ping message
        if (send_ping(sock_fd, packet, addr) <= 0) {
            perror("sendto");
        }

        // wait several milliseconds
        usleep(wait_time);

        // Get the response
        if (recv_ping(sock_fd, packet, addr) <= 0) {
            perror("recvfrom");
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