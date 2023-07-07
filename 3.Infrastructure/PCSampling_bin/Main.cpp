#include <signal.h>
#include <iostream>
#include <chrono>

#include "cv/VideoHelper.h"
#include "csi/CSIHelper.h"
#include "network/Network.h"
#include "storage/FileStorage.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/version.hpp>

#define BUFSIZE             4096
#define BROADCAST_PORT      8080


using namespace cv;

int             quit;               // quit flag
unsigned char   buf_addr[BUFSIZE];  // buffer for storing the received data
csi_struct*     csi_status;         // CSI status

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
 * @brief Record the CSI status
 * @param buf_addr
 * @param cnt
 * @param csi_status
 */
int main(int argc, char* argv[])
{
    /* If the user doesn't specify the port, use the default 8080 */
    int port = BROADCAST_PORT;
    int width = 320;
    int height = 240;

    if (argc == 2) {
        port = atoi(argv[1]);
    } else if (argc == 4) {
        port = atoi(argv[1]);
        width = atoi(argv[2]);
        height = atoi(argv[3]);
    } else {
        std::cout << "Usage: " << argv[0] << " [port] [width] [height]" << std::endl;
        return 0;
    }

    /* Open the OpenCV VideoCapture */
    VideoCapture cap = VideoHelper::openCamera();

#ifdef CV_VERSION_EPOCH
    // If using the older version of OpenCV, use the following code
    auto codec = CV_FOURCC('M', 'J', 'P', 'G');

    /* Set the video resolution to width x height */
    cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);

#else
    /* New version API */
    auto codec = VideoWriter::fourcc('M', 'J', 'P', 'G');

    /* Set the video resolution to width x height */
    cap.set(CAP_PROP_FRAME_WIDTH, width);
    cap.set(CAP_PROP_FRAME_HEIGHT, height);

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
    sge::FileStorage fs("csi");

    /* size of the file */
    unsigned long file_size = 0;
    unsigned long last_file_size = 0;

    /* Keep listening to the kernel and waiting for the csi report */
    while(!quit) {

        /* Get the video frame */
        Mat frame;
        cap >> frame;

        /* Convert the RGB frame to grayscale */
#ifdef CV_VERSION_EPOCH
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
            size_t frame_size = frame.total() * frame.elemSize();

            /* Compute the total size */
            size_t total_size = sizeof(long) * 3 + sizeof(now_ms) + frame_size + recvd + 2;

            /* Write the data into file
             * total_size | frame_size | csi_size | timestamp | frame | csi_data | \r\n
            */
            fs.write((unsigned char*)&total_size, sizeof(total_size));   // total size
            fs.write((unsigned char*)&frame_size, sizeof(frame_size));   // frame size
            fs.write((unsigned char*)&recvd, sizeof(recvd));             // csi size
            fs.write((unsigned char*)&now_ms, sizeof(now_ms));           // timestamp
            fs.write((unsigned char*)frame.data, frame_size);            // frame
            fs.write((unsigned char*)buf_addr, recvd);                   // csi data
            fs.write((unsigned char*)"\r\n", 2);                         // \r\n

//            /* Check the file size, if it is continuously increasing, print the increasing rate */
//            file_size = bin.tellp();
//            if (file_size > last_file_size) {
//                print_status(csi_status, file_size - last_file_size);
//                last_file_size = file_size;
//            }
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
    close(sockfd);

    /* Free the memory */
    free(csi_status);

    /* Exit the program */
    return 0;
}
