#include <csignal>
#include <iostream>
#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>

#include "cv/VideoHelper.h"
#include "csi/CSIHelper.h"
#include "network/Network.h"
#include "fs/SynchronousBinProcessor.h"
#include "local/Keyboards.h"

#define BUF_SIZE            4096
#define BROADCAST_PORT      8080

using namespace cv;

int                 quit;               // quit flag
int                 sock_fd;            // socket file descriptor
unsigned char       temp_buf[BUF_SIZE]; // buffer for storing the received data

bool                b_video_capture_task;   // flag for continuing the loop


/**
 * @brief Handle the Ctrl+C signal
 * @param sig
 */
void sig_handler(int signal) {
    if (signal == SIGINT) {
        // log the received data for off-line processing
        std::cout << "Received SIGINT. Quitting..." << std::endl;

        // close the log file
        quit = 1;
    }
}

/**
 * @brief Initialize the parameters
 * 
 * @param port 
 * @param cap 
 */
void init_params(int port, VideoCapture& cap) {

    // Register the signal handler: ctrl + c
    signal(SIGINT, sig_handler);

    // Initialize the quit flag
    quit = 0;

    // Create the socket
    sock_fd = create_broadcast_socket();

    // Bind the socket
    bind_addr(sock_fd, port);

    // Initialize the quit flag
    quit = 0;
}


/**
 * @brief Release the parameters
 * 
 * @param cap 
 * @param fd
 * @param csi
 */
void release_params(VideoCapture& cap, int fd) {

    // Wait b_video_capture_task to be false
    while (b_video_capture_task) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Close the video capture
    VideoHelper::closeCamera(cap);

    // Close the socket
    close(fd);
}


/**
 * @brief Get the video frame
 * 
 * @param arg 
 */
void video_capture_task(void *arg) {

    // Convert the arg to the VideoCapture
    auto *cap = (VideoCapture *)arg;

    // Set the b_thread_running flag to be true
    b_video_capture_task = true;

    // Loop until the user presses the ESC key
    while (!quit) {
        // Get the video frame
        Mat cv_frame;
        *cap >> cv_frame;

        // If the frame is empty, skip it
        if (cv_frame.empty()) {
            continue;
        }

        // If the frame is not 720p, resize it to 720p
        if (cv_frame.rows != 720 || cv_frame.cols != 1280) {
            resize(cv_frame, cv_frame, Size(1280, 720));
        }

        // Display the video frame
        imshow("Real-time VideoCapture", cv_frame);

        // If the user presses the ESC key, quit the program
        if (waitKey(KEYBOARD_Q) == 27) {
            quit = 1;
        }
    }

    b_video_capture_task = false;
}


/**
 * @brief The main function
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
    // If the user doesn't specify the port, use the default 8080
    int port;

    // Parse the command line arguments
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
        return 0;
    } else if (argc == 1) {
        port = BROADCAST_PORT;
    } else if (argc == 2) {
        port = static_cast<int>(strtol(argv[1], nullptr, 10));
    } else {
        std::cout << "Invalid arguments. Use -h for help." << std::endl;
        exit(1);
    }

    // Create the video capture
     VideoCapture cap = VideoHelper::openCamera();

    // Set the camera resolution
    VideoHelper::setCameraResolution(cap, 1280, 720);

    // Initialize the parameters
    init_params(port, cap);

    // Create the thread pool with one thread
    std::thread video_capture_thread(video_capture_task, &cap);

    // Start the thread
    video_capture_thread.detach();

    // Print the waiting message
    printf("Waiting for the first packet...\n");

    // Keep listening to the kernel and waiting for the csi report
    while(!quit) {

        // Get the CSI data from the UDP broadcast
        struct sockaddr_in addr_in{};
        socklen_t senderLen = sizeof(addr_in);
        ssize_t received_size = recvfrom(sock_fd, temp_buf, BUF_SIZE, 0,
                                         (struct sockaddr*)&addr_in, &senderLen);

        if (received_size > 0) {
            // TODO: Process the received data
        }
    }

    // Release the resources
    release_params(cap, sock_fd);

    // Exit the program
    return 0;
}
