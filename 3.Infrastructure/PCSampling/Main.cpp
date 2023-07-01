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
#include <opencv2/core/version.hpp>

#define BUFSIZE             4096
#define BROADCAST_PORT      8080
#define BIN_FILENAME        "csi_videoc.dat"

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
    if (signo == SIGQUIT) {
        /* log the received data for off-line processing */
        std::cout << "Received SIGQUIT. Quitting..." << std::endl;

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


void write_data(std::ofstream &file, unsigned char* buf_addr, int size)
{
    if (!file.is_open()) {
        perror("File is not open");
        return;
    }

    /* Write the data to the file */
    while (true) {
        file.write((char*)buf_addr, size);
        file.flush();

        if (!file.fail()) {
            break;
        }

        /* Reopen the file */
        file.close();
        file.open(BIN_FILENAME, std::ios::out | std::ios::binary | std::ios::app);
        if (!file.is_open()) {
            perror("File is not open");
            return;
        }
    }
}


void print_status(csi_struct *package, int increase_size)
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
    printf("Increased size: %.2f KB\n\n", increase_size / 1024.0);
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

    /* Open the OpenCV VideoCapture */
    VideoCapture cap = VideoHelper::openCamera();

#if CV_VERSION_EPOCH == 2
    // If using the older version of OpenCV, use the following code
    auto codec = CV_FOURCC('M', 'J', 'P', 'G');

    /* Set the video resolution to 320 x 240 */
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

    /* Set to grayscale mode */
//    cap.set(CV_CAP_PROP_CONVERT_RGB, 0);
#else
    #if CV_VERSION_MAJOR >= 4
        /* New version API */
        auto codec = VideoWriter::fourcc('M', 'J', 'P', 'G');

        /* Set the video resolution to 320 x 240 */
        cap.set(CAP_PROP_FRAME_WIDTH, 320);
        cap.set(CAP_PROP_FRAME_HEIGHT, 240);

        /* Set to grayscale mode */
//        cap.set(CAP_PROP_CONVERT_RGB, 0);
    #endif
#endif

    /* Create the socket */
    int sockfd = create_broadcast_socket();

    /* Bind the socket */
    bind_addr(sockfd, port);

    /* Allocate memory for the status struct */
    csi_status = (csi_struct*)malloc(sizeof(csi_struct));

    /* Register the signal handler: ctrl + d */
    signal(SIGQUIT, sig_handler);

    /* Initialize the quit flag */
    quit = 0;

    /* Initialize the received socket address */
    struct sockaddr_in senderAddr;
    socklen_t senderLen = sizeof(senderAddr);

    /* Print the waiting message */
    printf("Waiting for the first packet...\n");

    /* Try to record the video frame and the CSI status */
    std::ofstream bin(BIN_FILENAME, std::ios::out | std::ios::binary);
    if (!bin.is_open()) {
        std::cout << "Failed to open the file" << std::endl;
        return -1;
    }

    /* size of the file */
    unsigned long file_size = 0;
    unsigned long last_file_size = 0;

    /* Keep listening to the kernel and waiting for the csi report */
    while(!quit) {

        /* Get the video frame */
        Mat frame;
        cap >> frame;

        /* Convert the RGB frame to grayscale */
#if CV_VERSION_EPOCH == 2
        cvtColor(frame, frame, CV_RGB2GRAY);
#else
        cvtColor(frame, frame, COLOR_RGB2GRAY);
#endif

        /* keep listening to the kernel and waiting for the csi report */
        long recvd = recvfrom(sockfd, buf_addr, BUFSIZE, 0,
                             (struct sockaddr*)&senderAddr, &senderLen);

        if (recvd > 0){

            /* fill the status struct with information about the rx packet */
            record_status(buf_addr, recvd, csi_status);

            /* Get current timestamp */
            auto now = std::chrono::system_clock::now();
            auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

            /* Get the video frame size */
            unsigned long frame_size = frame.total() * frame.elemSize();

            /* Compute the total size */
            unsigned long total_size = sizeof(long) * 3 + sizeof(now_ms) + frame_size + recvd + 2;

            /* Write the data into file
             * total_size | frame_size | csi_size | timestamp | frame | csi_data | \r\n
            */
            write_data(bin, (unsigned char*)&total_size, sizeof(total_size));
            write_data(bin, (unsigned char*)&frame_size, sizeof(frame_size));
            write_data(bin, (unsigned char*)&recvd, sizeof(recvd));
            write_data(bin, (unsigned char*)&now_ms, sizeof(now_ms));
            write_data(bin, (unsigned char*)frame.data, frame_size);
            write_data(bin, (unsigned char*)buf_addr, recvd);
            write_data(bin, (unsigned char*)"\r\n", 2);

            /* Check the file size, if it is continuously increasing, print the increasing rate */
            file_size = bin.tellp();
            if (file_size > last_file_size) {
                print_status(csi_status, file_size - last_file_size);
                last_file_size = file_size;
            }
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

    /* Close the video capture */
    cap.release();

    /* Close the socket */
    close(sockfd);

    /* Free the memory */
    free(csi_status);

    /* Exit the program */
    return 0;
}
