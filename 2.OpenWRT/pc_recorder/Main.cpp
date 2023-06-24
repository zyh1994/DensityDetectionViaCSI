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

#define BUFSIZE 4096
#define BROADCAST_PORT 8080

int quit;
unsigned char buf_addr[BUFSIZE];
unsigned char data_buf[1500];

COMPLEX csi_matrix[3][3][114];
csi_struct*   csi_status;

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


void print_csi_status(csi_struct *package)
{
    /**
     * An example of the output:
     * csi_status->tstamp: 108
     * csi_status->buf_len: 11525
     * csi_status->channel: 40457
     * csi_status->rate: 149
     * csi_status->rssi: 52
     * csi_status->rssi_0: 50
     * csi_status->rssi_1: 41
     * csi_status->rssi_2: 46
     * csi_status->payload_len: 10240
     * csi_status->csi_len: 60420
     * csi_status->phyerr: 0
     * csi_status->noise: 0
     * csi_status->nr: 3
     * csi_status->nc: 3
     * csi_status->num_tones: 56
     * csi_status->chanBW: 0
     */

    /* Clear the screen */
    printf("\033[2J");

    /* Print the CSI status */
    printf("csi_status->tstamp: %ld\n", package->tstamp);
    printf("csi_status->buf_len: %d\n", package->buf_len);
    printf("csi_status->channel: %d\n", package->channel);
    printf("csi_status->rate: %d\n", package->rate);
    printf("csi_status->rssi: %d\n", package->rssi);
    printf("csi_status->rssi_0: %d\n", package->rssi_0);
    printf("csi_status->rssi_1: %d\n", package->rssi_1);
    printf("csi_status->rssi_2: %d\n", package->rssi_2);
    printf("csi_status->payload_len: %d\n", package->payload_len);
    printf("csi_status->csi_len: %d\n", package->csi_len);
    printf("csi_status->phyerr: %d\n", package->phyerr);
    printf("csi_status->noise: %d\n", package->noise);
    printf("csi_status->nr: %d\n", package->nr);
    printf("csi_status->nc: %d\n", package->nc);
    printf("csi_status->num_tones: %d\n", package->num_tones);
    printf("csi_status->chanBW: %d\n", package->chanBW);
}


/**
 * @brief Record the CSI status
 * @param buf_addr
 * @param cnt
 * @param csi_status
 */
int main(int argc, char* argv[])
{
    /* If the user doesn't specify the port, use the default 8080 */
    int port = BROADCAST_PORT;

    if (argc == 2) {
        port = atoi(argv[1]);
    }

    /* file pointer */
//    FILE*       fp;

    int         i;
    int         total_msg_cnt,cnt;
//    int         log_flag;
//    unsigned char endian_flag;
//    u_int16_t   buf_len;

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

//    log_flag = 1;
    csi_status = (csi_struct*)malloc(sizeof(csi_struct));

    /* Register the signal handler */
    signal(SIGINT, sig_handler);

    quit = 0;
    total_msg_cnt = 0;

    struct sockaddr_in senderAddr;
    socklen_t senderLen = sizeof(senderAddr);

    printf("Waiting for the first packet...\n");

//    unsigned char udp_buf[BUFSIZE] = {0};

    while(!quit) {

//        std::cout << "1" << std::endl;

        /* keep listening to the kernel and waiting for the csi report */
        cnt = recvfrom(sockfd, buf_addr, BUFSIZE, 0, (struct sockaddr*)&senderAddr, &senderLen);

        if (cnt > 0){
            total_msg_cnt += 1;

            /* fill the status struct with information about the rx packet */
            record_status(buf_addr, cnt, csi_status);

            /* Print the information of the CSI packet */
            print_csi_status(csi_status);

            /* 
             * fill the payload buffer with the payload
             * fill the CSI matrix with the extracted CSI value
             */
            record_csi_payload(buf_addr, csi_status, data_buf, csi_matrix);

//            printf("Recv %dth msg with rate: 0x%02x | payload len: %d\n",
//                total_msg_cnt,
//                csi_status->rate,
//                csi_status->payload_len);
            
            /* log the received data for off-line processing */
//            if (log_flag){
//                buf_len = csi_status->payload_len;
//                fwrite(&buf_len,1,2,fp);
//                fwrite(buf_addr,1,buf_len,fp);
//            }
        }
    }

//    fclose(fp);
    close(sockfd);
    free(csi_status);
    return 0;
}
