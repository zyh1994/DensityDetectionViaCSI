#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "VideoHelper.h"

#define BUFSIZE 4096
#define PORT 12345 // 你可以选择你的端口号

int quit;
unsigned char buf_addr[BUFSIZE];
unsigned char data_buf[1500];

COMPLEX csi_matrix[3][3][114];
csi_struct*   csi_status;

void sig_handler(int signo)
{
    if (signo == SIGINT)
        quit = 1;
}

int main(int argc, char* argv[])
{
    FILE*       fp;
    int         i;
    int         total_msg_cnt,cnt;
    int         log_flag;
    unsigned char endian_flag;
    u_int16_t   buf_len;

    int sockfd;
    struct sockaddr_in server_addr, client_addr;

    log_flag = 1;
    csi_status = (csi_struct*)malloc(sizeof(csi_struct));

    // 创建socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 绑定socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        exit(1);
    }

    printf("#Receiving data! Press Ctrl+C to quit!\n");

    quit = 0;
    total_msg_cnt = 0;
    socklen_t addr_len = sizeof(client_addr);

    while(1) {
        if (1 == quit){
            return 0;
            fclose(fp);
            close(sockfd);
        }

        /* keep listening to the kernel and waiting for the csi report */
        cnt = recvfrom(sockfd, buf_addr, BUFSIZE, 0, (struct sockaddr*)&client_addr, &addr_len);

        if (cnt > 0){
            total_msg_cnt += 1;

            /* fill the status struct with information about the rx packet */
            record_status(buf_addr, cnt, csi_status);

            /* 
             * fill the payload buffer with the payload
             * fill the CSI matrix with the extracted CSI value
             */
            record_csi_payload(buf_addr, csi_status, data_buf, csi_matrix); 

            printf("Recv %dth msg with rate: 0x%02x | payload len: %d\n",total_msg_cnt,csi_status->rate,csi_status->payload_len);
            
            /* log the received data for off-line processing */
            if (log_flag){
                buf_len = csi_status->buf_len;
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
