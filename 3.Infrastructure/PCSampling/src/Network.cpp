//
// Created by Seagosoft on 7/1/2023.
//

#include "../inc/Network.h"

/**
 * @brief Create a UDP socket for receiving broadcast packets
 * @return
 */
int create_broadcast_socket()
{
    /* Set the socket */
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    /* Set the socket options */
    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
                   &broadcastEnable, sizeof(broadcastEnable)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    /* Set the socket as non-blocking */
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        exit(1);
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("Failed to set socket as non-blocking");
        exit(1);
    }

    return sockfd;
}


/**
 * @brief Bind the socket to the specified port
 * @param sockfd
 * @param port
 */
void bind_addr(int sockfd, int port)
{
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(port);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr*)&localAddr, sizeof(localAddr)) == -1) {
        perror("bind");
        exit(1);
    }
}