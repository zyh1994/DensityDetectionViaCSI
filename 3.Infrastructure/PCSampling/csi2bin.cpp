#include <csignal>
#include <iostream>
#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>

#include "bin/SynchronousBinProcessor.h"
#include "csi/OpenWRT_v1.h"
#include "csi/StandardCSIDataProcessing.h"
#include "network/Network.h"


#define BROADCAST_PORT      8080

using namespace             cv;
unsigned char               temp_buf[4096];     // buffer for storing the received data
bool                        b_quit;             // flag for quitting the program
BinProcessor                processor;          // create the synchronous bin processor


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

    // Open camera
    VideoCapture cap(0);
    Mat frame;

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

        // Save the frame to the bin processor
        processor.append_data(frame);

// if not _DEBUG mode, enable the following code
#if not _DEBUG
        // Display the video frame
        imshow("Real-time VideoCapture", frame);
#endif
        // Wait for 30ms
        waitKey(30);
    }

    // Close all the windows
    destroyAllWindows();
}


/**
 * @brief Get the CSI data
 */
void csi_sampling_task() {

    // Create the socket
    int sock_fd = create_broadcast_socket();

    // Bind the socket
    bind_addr(sock_fd, BROADCAST_PORT);

    // CSI Standard Data Processing Class for processing the CSI data
    size_t received_size = 0;
    CSIStandardDataProcessingClass csiHandeler;

    // Socket address
    struct sockaddr_in addr_in{};

    // Loop until the user presses the ESC key
    while (!b_quit) {

#if _DEBUG
        // Generate the Fake CSI data
        gen_fake_data(temp_buf, received_size);
#else
        // Receive the CSI data from the socket
        socklen_t senderLen = sizeof(addr_in);
        received_size = recvfrom(sock_fd, temp_buf, BUF_SIZE, 0, (struct sockaddr*)&addr_in, &senderLen);
#endif
        // Process the CSI data
        if (received_size > 0) {

            // Process the CSI data
            csiHandeler.updateWithOpenWRTv1(temp_buf, received_size);

            // Get the raw data from the CSI data
            auto* raw_data = csiHandeler.toBytes(received_size);

            // Save the CSI data to the bin processor
            processor.append_data(raw_data, received_size);
        }

#if _DEBUG
        // Sleep for 10ms
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif
    }

#if not _DEBUG
    // Close the socket
    close(sock_fd);
#endif
}


/**
 * @brief The main function
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
#if not _DEBUG
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
#endif

    // Register the signal handler ctrl + c
    signal(SIGINT, sig_handler);

    // Set the flag 
    b_quit = false;

    // Create the thread for video capture
    std::thread video_capture_thread(video_capture_task);

    // Create the thread for CSI sampling
    std::thread csi_sampling_thread(csi_sampling_task);

#if _DEBUG
    // This thread will run 1 minute 30 seconds
    std::this_thread::sleep_for(std::chrono::seconds(70));
    b_quit = true;
#else
    // Keep listening to the kernel and waiting for the csi report
    while(!b_quit) {

        // Wait for 1ms
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
#endif

    // Wait all the threads to finish
    video_capture_thread.join();
    csi_sampling_thread.join();

    // Exit the program
    return 0;
}
