#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <chrono>

#include "VideoHelper.h"
#include "CSIHelper.h"

#include <opencv2/opencv.hpp>

#define BUFSIZE 4096
#define BROADCAST_PORT 8080

using namespace cv;

int             quit;               // quit flag
unsigned char   buf_addr[BUFSIZE];  // buffer for storing the received data

//COMPLEX csi_matrix[3][3][114];
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

    /* Open the OpenCV VideoCapture */
    VideoCapture cap = VideoHelper::openCamera();

    /* Set the video resolution to 640x480 */
    auto codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
    VideoWriter writer = VideoHelper::openVideoWriter("output.avi", codec,
                                                      30.0, Size(640, 480));

    /* Create the socket */
    int sockfd = create_broadcast_socket();

    /* Bind the socket */
    bind_addr(sockfd, port);

    /* Allocate memory for the status struct */
    csi_status = (csi_struct*)malloc(sizeof(csi_struct));

    /* Register the signal handler */
    signal(SIGINT, sig_handler);

    /* Initialize the quit flag */
    quit = 0;

    /* Initialize the received socket address */
    struct sockaddr_in senderAddr;
    socklen_t senderLen = sizeof(senderAddr);

    /* Print the waiting message */
    printf("Waiting for the first packet...\n");

    /* Try to record the video frame and the CSI status */
    std::ofstream bin("frame_hash.bin", std::ios::out | std::ios::binary);
    if (!bin.is_open()) {
        std::cout << "Failed to open the file" << std::endl;
        return -1;
    }

    /* Keep listening to the kernel and waiting for the csi report */
    while(!quit) {

        /* Get the video frame */
        Mat frame;
        cap >> frame;

        /* keep listening to the kernel and waiting for the csi report */
        int recvd = recvfrom(sockfd, buf_addr, BUFSIZE, 0,
                             (struct sockaddr*)&senderAddr, &senderLen);

        if (recvd > 0){

            /* fill the status struct with information about the rx packet */
            record_status(buf_addr, recvd, csi_status);

            /* Print the information of the CSI packet */
            print_csi_status(csi_status);

            /* Get current timestamp */
            auto now = std::chrono::system_clock::now();
            auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

            /* Get the video frame size */
            unsigned long frame_size = frame.total() * frame.elemSize();

            /* Get the CSI data size */
            unsigned long csi_size = csi_status->buf_len;

            /* Compute the total size */
            unsigned long total_size = frame_size + csi_size + sizeof(frame_size) + sizeof(csi_size) +
                    sizeof(total_size) + sizeof(now_ms) + 2;

            /* Write the data into file
             * total_size | frame_size | csi_size | timestamp | frame | csi_data | \r\n
            */
            bin.write((char*)&total_size, sizeof(total_size));
            bin.write((char*)&frame_size, sizeof(frame_size));
            bin.write((char*)&csi_size, sizeof(csi_size));
            bin.write((char*)&now_ms, sizeof(now_ms));
            bin.write((char*)frame.data, frame_size);
            bin.write((char*)buf_addr, csi_size);
            bin.write("\r\n", 2);
        }

        /* Display the video frame */
        imshow("Real-time VideoCapture", frame);

        /* If the user presses the ESC key, quit the program */
        if (waitKey(1) == 27) {
            quit = 1;
        }
    }

    /* Close the file */
    bin.close();

    /* Close the video writer */
    writer.release();

    /* Close the video capture */
    cap.release();

    /* Close the socket */
    close(sockfd);

    /* Free the memory */
    free(csi_status);

    /* Exit the program */
    return 0;
}
