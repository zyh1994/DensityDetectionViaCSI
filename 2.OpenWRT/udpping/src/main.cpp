#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"

#define MAXBUFSIZE 1024
#define PORT 5000
#define BROADCAST_IP "192.168.1.255"

int main(void) 
{
    NetworkInterface interface;
    get_first_non_loopback_interface(&interface);

    printf("Interface name: %s\n", interface.name.c_str());
    printf("Host address: %s\n", interface.hostAddr.c_str());
    printf("Netmask: %s\n", interface.netmask.c_str());

    // int sockfd;
    // struct sockaddr_in broadcastAddr;
    // char *sendString = "Hello, Network!";  
    // int broadcastPermission = 1;

    // /* Create a best-effort datagram socket using UDP */
    // if ((sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    //     perror("socket");
    //     exit(EXIT_FAILURE);
    // }

    // /* Set socket to allow broadcast */
    // if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0) {
    //     perror("setsockopt");
    //     exit(EXIT_FAILURE);
    // }

    // /* Construct local address structure */
    // memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    // broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    // broadcastAddr.sin_addr.s_addr = inet_addr(BROADCAST_IP);/* Broadcast IP address */
    // broadcastAddr.sin_port = htons(PORT);               /* Broadcast port */

    // /* Broadcast sendString in datagram to clients every 3 seconds*/
    // for (;;) { 
    //     if (sendto(sockfd, sendString, strlen(sendString), 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) != strlen(sendString)) {
    //         perror("sendto");
    //         exit(EXIT_FAILURE);
    //     }

    //     sleep(3);   /* Avoids flooding the network */
    // }

    // /* not reached but good form to have it here */
    // close(sockfd);

    return 0;
}
