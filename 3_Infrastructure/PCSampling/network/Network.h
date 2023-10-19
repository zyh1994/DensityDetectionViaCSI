//
// Created by Seagosoft on 7/1/2023.
//

#ifndef PCSAMPLING_NETWORK_H
#define PCSAMPLING_NETWORK_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

extern int create_broadcast_socket();

extern void bind_addr(int sockfd, int port);

#endif //PCSAMPLING_NETWORK_H
