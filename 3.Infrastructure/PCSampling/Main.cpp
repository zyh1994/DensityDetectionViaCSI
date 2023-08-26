#include <csignal>
#include <iostream>
#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>

#include "cv/VideoHelper.h"
#include "csi/CSIHelper.h"
#include "network/Network.h"
#include "fs/SynchronousBinProcessor.h"

#define BUF_SIZE            4096
#define BROADCAST_PORT      8080

using namespace cv;

unsigned char       temp_buf[BUF_SIZE]; // buffer for storing the received data
bool                b_quit;             // flag for quitting the program


SynchronousBinProcessor bin_processor;  // create the synchronous bin processor

/**
 * @brief Handle the Ctrl+C signal
 * @param sig
 */
void sig_handler(int signal) {
    if (signal == SIGINT) {
        // log the received data for off-line processing
        std::cout << "Received SIGINT. Quitting..." << std::endl;

        // close the log file
        b_quit = true;
    }
}


/**
 * @brief Get the video frame
 */
void video_capture_task() {

    // Create the video capture
     VideoCapture cap = VideoHelper::openCamera();

    // Set the camera resolution
    VideoHelper::setCameraResolution(cap, 1280, 720);

    // Get the video frame
    cv::Mat frame;

    // Loop until the user presses the ESC key
    while (!b_quit) {

        // Get the video frame
        cap >> frame;

        // If the frame is empty, skip it
        if (frame.empty()) {
            continue;
        }

        // If the frame is not 720p, resize it to 720p
        if (frame.rows != 720 || frame.cols != 1280) {
            resize(frame, frame, Size(1280, 720));
        }

        // Display the video frame
        imshow("Real-time VideoCapture", frame);

        // Wait for 30ms
        waitKey(30);
    }

    // Close all the windows
    destroyAllWindows();

    // Close the video capture
    VideoHelper::closeCamera(cap);
}


/**
 * @brief Get the CSI data
 */
void csi_sampling_task() {

    // Create the socket
    int sock_fd = create_broadcast_socket();

    // Bind the socket
    bind_addr(sock_fd, BROADCAST_PORT);

    // Loop until the user presses the ESC key
    while (!b_quit) {

        // Get the CSI data from the UDP broadcast
        struct sockaddr_in addr_in{};
        socklen_t senderLen = sizeof(addr_in);
        ssize_t received_size = recvfrom(sock_fd, temp_buf, BUF_SIZE, 0,
                                        (struct sockaddr*)&addr_in, &senderLen);

        // If the received data is valid
        if (received_size > 0) {
            bin_processor.append_data(temp_buf, received_size);
        }
    }

    // Close the socket
    close(sock_fd);
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

    // Register the signal handler ctrl + c
    signal(SIGINT, sig_handler);

    // Set the flag 
    b_quit = false;
    
    // Create the thread for video capture
    std::thread video_capture_thread(video_capture_task);

    // Create the thread for CSI sampling
    std::thread csi_sampling_thread(csi_sampling_task);

    // Print the waiting message
    printf("Waiting for the first packet...\n");

    // Keep listening to the kernel and waiting for the csi report
    while(!b_quit) {

        // Wait for 1ms
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Wait all the threads to finish
    video_capture_thread.join();
    csi_sampling_thread.join();

    // Exit the program
    return 0;
}
