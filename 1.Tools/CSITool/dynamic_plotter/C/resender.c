#include "MD5.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
// #include <linux/connector.h>
#include "linux_connector.h"


#define MAX_PAYLOAD     2048
#define SLOW_MSG_CNT    1
#define SERVER_PORT     8090
#define SEND_WHOLE_BUF  false


#ifdef CN_NETLINK_USERS
#undef CN_NETLINK_USERS
#define CN_NETLINK_USERS		11	/* Highest index + 1 */
#endif

#define CN_IDX_IWLAGN   (CN_NETLINK_USERS + 0xf)
#define CN_VAL_IWLAGN   0x1

FILE* open_file(char* filename, char* spec);    // Open a file and check for errors
void exit_program(int code);                    // Exit the program
void exit_program_err(int code, char* func);    // Exit the program with an error
void data_reader();                             // Read data from the "csi.dat" file and process each CSI frame

// Socket descriptor
int sock_client_fd = -1;  

// void setup_tcp_client()
// {
//     // create a socket
//     sock_client_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sock_client_fd < 0) {
//         perror("socket creation failed");
//     }

//     // set the server address and port number
//     struct sockaddr_in server;
//     server.sin_addr.s_addr = inet_addr("localhost");
//     server.sin_family = AF_INET;
//     server.sin_port = htons(SERVER_PORT);

//     // connect to the server
//     if (connect(sock_client_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
//         perror("connect failed");
//     }
// }

void Inet_pton(int family, const char * ip, void * addptr)
{
	int n;
	if ( (n = inet_pton(family, ip, addptr)) <=0)
	{
		if(n == 0)
			fprintf(stdin, "inet_pton format error : %s\n", ip);
		else
			fprintf(stdin, "inet_pton error : %s\n", ip);
		exit_program(1);
	}
}

void setup_tcp_client()
{
    // set the server address and port number
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    Inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    // create a socket
    sock_client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_client_fd < 0) {
        perror("socket creation failed");
    }

    // connect to the server
    if (connect(sock_client_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed");
    }
}


int main(int argc, char** argv)
{
    // Setup the TCP connection (server for remitting the CSI data)
    // setup_tcp_client();

    // Read data from the "csi.dat" file and process each CSI frame
    data_reader();

    // Exit the program
    // exit_program(0);
}

// Read data from the "csi.dat" file and process each CSI frame
void data_reader()
{
    // Open the "csi.dat" file for reading
    FILE *in = open_file("csi.dat", "r");
    if (in == NULL) {
        perror("Error opening csi.dat for reading");
        exit(1);
    }

    // MD5 hash of the data buffer
    uint8_t md5_hashstr[16];

    // Temporary buffer for reading the data
    unsigned char temp_buffer[MAX_PAYLOAD * 2]; 

    // Clear the temporary buffer
    memset(temp_buffer, 0, sizeof(temp_buffer));

    // Data length
    unsigned short data_len = 0;

    // Read and process each frame of CSI data from the file
    while (!feof(in))
    {
        // Clear the temporary buffer
        memset(temp_buffer, 0, sizeof(temp_buffer));

        // Read the length of the message
        if (fread(&data_len, 1, sizeof(unsigned short), in) != sizeof(unsigned short)) {
            if (feof(in)) {
                break;
            }
            perror("Error reading data_len from file");
            exit(1);
        }

        // print out the data in hex
        printf("Data length in hex: ");
        for (int i = 0; i < sizeof(unsigned short); i++)
            printf("%2.2x", ((unsigned char*)&data_len)[i]);
        printf(" md5: ");

        // Convert data_len to host byte order
        data_len = ntohs(data_len);

        // Read the CSI data
        // read_bytes = ;
        // printf("read_bytes = %d\n", read_bytes);
        // printf("data_len = %d\n", data_len);
        if (data_len != fread(temp_buffer, 1, data_len, in)) {
            perror("Error reading data_buffer from file");
            exit(1);
        }

#if SEND_WHOLE_BUF

        // Send the data to the server
        if (send(sock_client_fd, temp_buffer, data_len, 0) < 0) {
            perror("send failed");
        }

        // Compute the MD5 hash of the data
        md5((uint8_t*)temp_buffer, data_len, md5_hashstr);
        for (int i = 0; i < 16; i++)
            printf("%2.2x", md5_hashstr[i]);
        printf("\n");
#else
        // Convert the data to network byte order
        struct cn_msg *cmsg = (struct cn_msg *)temp_buffer;

        // Get the length of the data
        unsigned short l = cmsg->len;
        unsigned short l2 = htons(l);

        // Send the data to the server
        // if (send(sock_client_fd, &l2, sizeof(unsigned short), 0) < 0) {
        //     perror("send failed");
        // }
        // if (write(sock_client_fd, &l2, sizeof(unsigned short)) < 0) {
            // perror("send failed");
        // }

        // // print out debug message
        // printf("sent size = %d md5: ", l2);

        // Compute the MD5 hash of the data
        md5((uint8_t*)temp_buffer, data_len, md5_hashstr);
        for (int i = 0; i < 16; i++)
            printf("%2.2x", md5_hashstr[i]);
        printf("\n");

        // Write the message
        // if (send(sock_client_fd, cmsg->data, l, 0) < 0) {
        //     perror("send failed");
        // }
        // if (write(sock_client_fd, cmsg->data, l) < 0) {
            // perror("send failed");
        // }

        // printf("debug 4");

        // Compute the MD5 hash of the data
        // md5((uint8_t*)cmsg->data, l, md5_hashstr);

        // Print out the debug information
        // printf("Data sent: len = %d, md5 = ", l);
        // for (int i = 0; i < 16; i++)
        //     printf("%2.2x", md5_hashstr[i]);
        // printf("\n");

#endif

        // Sleep for 1 s
        // usleep(1000);
        sleep(1);

    }


    if (in)
    {
        fclose(in);
        in = NULL;
    }
    if (sock_client_fd != -1)
    {
        // close the socket
        close(sock_client_fd);
        sock_client_fd = -1;
    }
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


void exit_program(int code)
{
    exit(code);
}


void exit_program_err(int code, char* func)
{
    perror(func);
    exit(code);
}
