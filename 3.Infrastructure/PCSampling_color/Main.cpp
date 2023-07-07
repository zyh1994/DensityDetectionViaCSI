#include <csignal>
#include <iostream>
#include <chrono>

#include "cv/VideoHelper.h"
#include "csi/CSIHelper.h"
#include "network/Network.h"
#include "storage/FileStorage.h"

#include <opencv2/opencv.hpp>

#define BUF_SIZE            4096
#define BROADCAST_PORT      8080


using namespace cv;

int             quit;               // quit flag
unsigned char   buf_addr[BUF_SIZE]; // buffer for storing the received data
csi_struct*     csi_status;         // CSI status

/**
 * @brief Handle the Ctrl+C signal
 * @param sig
 */
void sig_handler(int signo) {
    if (signo == SIGQUIT) {
        /* log the received data for off-line processing */
        std::cout << "Received SIGQUIT. Quitting..." << std::endl;

        /* close the log file */
        quit = 1;
    }
}


/**
 * @brief Get the current time in milliseconds
 * @return
 */
long long milliseconds() {
    using namespace std::chrono;
    return duration_cast<std::chrono::milliseconds>(
            system_clock::now().time_since_epoch()).count();
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
    // int width = 320;
    // int height = 240;

    if (argc == 2) {
        port = static_cast<int>(strtol(argv[1], nullptr, 10)); // atoi(argv[1]
    } 
    // else if (argc == 4) {
    //     port = static_cast<int>(strtol(argv[1], nullptr, 10)); // atoi(argv[1]
    //     // width = static_cast<int>(strtol(argv[2], nullptr, 10)); // atoi(argv[2]
    //     // height = static_cast<int>(strtol(argv[3], nullptr, 10)); // atoi(argv[3]
    // } else {
    //     std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
    //     return 0;
    // }

    /* Open the OpenCV VideoCapture */
    VideoCapture cap = sge::VideoHelper::openCamera();

    /* Set the camera resolution */
    // sge::VideoHelper::setCameraResolution(cap, width, height);

    /* Create the socket */
    int sock_fd = create_broadcast_socket();

    /* Bind the socket */
    bind_addr(sock_fd, port);

    /* Allocate memory for the status struct */
    csi_status = (csi_struct*)malloc(sizeof(csi_struct));

    /* Register the signal handler: ctrl + d */
    signal(SIGQUIT, sig_handler);

    /* Initialize the quit flag */
    quit = 0;

    /* Initialize the received socket address */
    struct sockaddr_in sock_recv_addr_in{};
    socklen_t senderLen = sizeof(sock_recv_addr_in);

    /* Print the waiting message */
    printf("Waiting for the first packet...\n");

    /* Try to record the video frame and the CSI status */
    // sge::FileStorage fs("csi");
    sge::FileStorage fs;

    /* Keep listening to the kernel and waiting for the csi report */
    while(!quit) {

        /* Get the video frame */
        Mat frame;
        cap >> frame;

        /* keep listening to the kernel and waiting for the csi report */
        long recvd = recvfrom(sock_fd, buf_addr, BUF_SIZE, 0,
                              (struct sockaddr*)&sock_recv_addr_in, &senderLen);

        if (recvd > 0){

            /* Write the CSI status to the file */
            fs.write(milliseconds(), frame, buf_addr, recvd);
        }

        /* Display the video frame */
        imshow("Real-time VideoCapture", frame);

        /* If the user presses the ESC key, quit the program */
        if (waitKey(1) == 27) {
            quit = 1;
        }
    }

    /* Close the video capture */
    cap.release();

    /* Close the socket */
    close(sock_fd);

    /* Free the memory */
    free(csi_status);

    /* Exit the program */
    return 0;
}
