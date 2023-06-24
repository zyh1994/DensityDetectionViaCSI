#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>

#include "VideoHelper.h"
#include "CSIHelper.h"

#define BUFSIZE 4096                // buffer size
#define BROADCAST_PORT 8888         // 广播端口

int quit;                           // quit flag

unsigned char buf_addr[BUFSIZE];    // buffer address
unsigned char data_buf[1500];       // data buffer

COMPLEX csi_matrix[3][3][114];      // CSI matrix
csi_struct*   csi_status;           // CSI status

/**
 * @brief Handle the Ctrl+C signal
 * @param sig
 */
void sig_handler(int signo)
{
    if (signo == SIGINT) {
        /* log the received data for off-line processing */
        std::cout << "Received SIGINT. Quitting..." << std::endl;

        /* close the log file */
        quit = 1;
    }
}

/**
 * @brief Record the CSI status
 * @param buf_addr
 * @param cnt
 * @param csi_status
 */
int main(int argc, char* argv[])
{
    /* file pointer */
    FILE*       fp;

    int         i;
    int         total_msg_cnt,cnt;
    int         log_flag;
    unsigned char endian_flag;
    u_int16_t   buf_len;

    /* Set the socket */
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    /* Set the socket options */
    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
                   &broadcastEnable, sizeof(broadcastEnable)) == -1) {
        std::cerr << "Failed to set socket options." << std::endl;
        return 1;
    }

    /* Set the socket as non-blocking */
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("Failed to get socket flags");
        return 1;
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("Failed to set socket as non-blocking");
        return 1;
    }

    /* Bind the socket */
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(BROADCAST_PORT);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr*)&localAddr, sizeof(localAddr)) == -1) {
        std::cerr << "Failed to bind socket." << std::endl;
        return 1;
    }

    log_flag = 1;
    csi_status = (csi_struct*)malloc(sizeof(csi_struct));

    /* Register the signal handler */
    signal(SIGINT, sig_handler);

    quit = 0;
    total_msg_cnt = 0;

    struct sockaddr_in senderAddr;
    socklen_t senderLen = sizeof(senderAddr);

    printf("Waiting for the first packet...\n");

    while(!quit) {

        /* keep listening to the kernel and waiting for the csi report */
        cnt = recvfrom(sockfd, buf_addr, BUFSIZE, 0, (struct sockaddr*)&senderAddr, &senderLen);

        if (cnt > 0){
            total_msg_cnt += 1;

            /* fill the status struct with information about the rx packet */
            record_status(buf_addr, cnt, csi_status);

            /* 
             * fill the payload buffer with the payload
             * fill the CSI matrix with the extracted CSI value
             */
            record_csi_payload(buf_addr, csi_status, data_buf, csi_matrix); 

            printf("Recv %dth msg with rate: 0x%02x | payload len: %d\n",
                total_msg_cnt,
                csi_status->rate,
                csi_status->payload_len);
            
            /* log the received data for off-line processing */
            if (log_flag){
                buf_len = csi_status->payload_len;
                fwrite(&buf_len,1,2,fp);
                fwrite(buf_addr,1,buf_len,fp);
            }
        }
    }

    fclose(fp);
    close(sockfd);
    free(csi_status);
    return 0;
}
