#include <signal.h>
#include <vector>
#include <string>

#include "common.h"

#ifndef INFINITE_LOOP
#define INFINITE_LOOP true
#endif

// 计数器
int cnt=0;

// Linux signal handler
volatile sig_atomic_t infinite_loop = 1;

// Linux signal handler
void handle_signal(int signal)
{
    if (signal == SIGINT)
    {
        printf("Caught SIGINT signal, terminating...\n");
        infinite_loop = 0;
    }
}


void create_ping_dev_target(const char* addr, struct ping_dev_info* dev_info) {
    // Create the ICMP socket
    dev_info->sock_fd = create_icmp_sock();

    // Set the ICMP address struct
    set_icmp_addr_struct(&(dev_info->addr), addr);
}


// Ping the target device
void ping(struct ping_dev_info* dev_inf, int const pid, useconds_t wait_time) {


#if INFINITE_LOOP
    while (infinite_loop)
    {
#endif

        // set the ICMP header
        set_icmp_header(&(dev_inf->packet), cnt++, pid);

        // send the ping message
        if (send_ping(dev_inf) <= 0) {
            perror("sendto");
        }

        // wait several milliseconds
        usleep(wait_time);

        // receive the ping message
        if (recv_ping(dev_inf) <= 0) {
            perror("recvfrom");
        } else {
            printf("%d bytes from %s: icmp_seq=%d ttl=%d\n",
                   PACKETSIZE, inet_ntoa(dev_inf->addr.sin_addr), cnt, 255);
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
//    if (argc != 4 && argc != 3)
//    {
//        printf("Usage: ping <address> <sample_rate>\n");
//        return 1;
//    }
    // get the input parameters
//    const char *address = argv[1];
//    useconds_t sample_rate = 1000000 / static_cast<useconds_t>(atoi(argv[2]));

    // Register signal handler for SIGINT
//    signal(SIGINT, handle_signal);
//
//    std::vector<std::string> ip_addr_vector;
//    ip_addr_vector.push_back("8.8.8.8");
//    ip_addr_vector.push_back("8.8.4.4");
//
//
//    // Create the target device info
//    struct ping_dev_info dev_info;
//    create_ping_dev_target(address, &dev_info);
//
//    // start ping
//    ping(&dev_info, getpid(), sample_rate);

    return 0;
}