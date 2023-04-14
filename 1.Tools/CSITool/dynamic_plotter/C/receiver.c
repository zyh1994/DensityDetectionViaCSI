#include "MD5.h"
#include "linux_connector.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
// #include <linux/netlink.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_PAYLOAD     2048
#define SLOW_MSG_CNT    1
#define SERVER_PORT     8090
#define DEBUG           2

// #include <linux/connector.h>

#ifdef CN_NETLINK_USERS
#undef CN_NETLINK_USERS
#define CN_NETLINK_USERS		11	/* Highest index + 1 */
#endif

#define CN_IDX_IWLAGN   (CN_NETLINK_USERS + 0xf)
#define CN_VAL_IWLAGN   0x1

// Socket descriptor
int sock_client_fd = -1;               

// File pointer for the output file
FILE* out = NULL;


// Function prototypes
FILE* open_file(char* filename, char* spec);    // Open a file and check for errors
void caught_signal(int sig);                    // Signal handler
void exit_program(int code);                    // Exit the program
void exit_program_err(int code, char* func);    // Exit the program with an error
void setup_tcp_client();                        // Setup the TCP connection (server for remitting the CSI data)


// Receive data from the Netlink socket and store it in the data buffer
// Then send the data to the client
void data_receiver()
{
    // Create a new block of memory for the data buffer
    char temp_buffer[MAX_PAYLOAD * 2];
    memset(temp_buffer, 0, sizeof(temp_buffer));

    uint8_t result[16];

    // Open and check log file
    FILE* out = open_file("csi.dat", "w");

    // Your logic for thread 0 here.
    while (1)
    {
        // Receive from socket, and check for errors
        size_t recv_len = recv(sock_client_fd, temp_buffer, sizeof(temp_buffer), 0);
        if (recv_len == -1) {
            exit_program_err(-1, "recv from server failed");
        }

#if DEBUG == 2
        // Compute the MD5 hash of the data
        md5((uint8_t*)temp_buffer, recv_len, result);
        for (int i = 0; i < 16; i++)
            printf("%2.2x", result[i]);
        printf("\n");
#endif

        // Pull out the message portion
        // struct cn_msg *cmsg = NLMSG_DATA(temp_buffer);
        struct cn_msg *cmsg = (struct cn_msg *)temp_buffer;

        // Append the data to the file
        unsigned short l = (unsigned short) cmsg->len;
        unsigned short l2 = htons(l);

        // Write the length of the message
        fwrite(&l2, 1, sizeof(unsigned short), out);

        // Write the message
        fwrite(cmsg->data, 1, l, out);
    }

}


int main(int argc, char** argv)
{
    // Setup a tcp non-blocking server to send the csi data
    setup_tcp_client();

    // Data received from the TCP server
    data_receiver();

    // Exit the program
    exit_program(0);
}


FILE* open_file(char* filename, char* spec)
{
    FILE* fp = fopen(filename, spec);
    if (!fp)
    {
        perror("fopen");
        exit_program(1);
    }
    return fp;
}


void caught_signal(int sig)
{
    fprintf(stderr, "Caught signal %d\n", sig);
    exit_program(0);
}


void exit_program(int code)
{
    if (out)
    {
        fclose(out);
        out = NULL;
    }
    if (sock_client_fd != -1)
    {
        close(sock_client_fd);
        sock_client_fd = -1;
    }
    exit(code);
}


void exit_program_err(int code, char* func)
{
    perror(func);
    exit_program(code);
}


void setup_tcp_client()
{
    // create a socket
    sock_client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_client_fd < 0) {
        perror("socket creation failed");
    }

    // set the server address and port number
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    // connect to the server
    if (connect(sock_client_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed");
    }
}