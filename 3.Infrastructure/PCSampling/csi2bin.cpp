#include <csignal>
#include <iostream>
#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>

#include "bin/SynchronousBinProcessor.h"
#include "bin/GeneralUtils.h"
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

    // Check FPS
    int fps = 0;
    int FPS = 0;

    // Timestamp indicating the counting time
    auto start = std::chrono::system_clock::now();

    // Set the camera resolution to 720p
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CAP_PROP_FRAME_HEIGHT, 720);

    // Set the camera resolution to 480p 16:9
    //cap.set(CAP_PROP_FRAME_WIDTH, 640);
    //cap.set(CAP_PROP_FRAME_HEIGHT, 480);

    // Loop until the user presses the ESC key
    while (!b_quit) {

        // Get the video frame
        cap >> frame;

        // If the frame is empty, skip it
        if (frame.empty()) {
            continue;
        }

        // Save the frame to the bin processor
        processor.append_data(frame);

        // Calculate the FPS
        fps++;

        // Get the current time
        auto end = std::chrono::system_clock::now();

        // Calculate the time difference
        auto diff = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

        // If the time difference is greater than 1 second
        if (diff >= 1) {

            // Update the FPS
            FPS = fps;

            // Reset the FPS
            fps = 0;

            // Reset the start time
            start = std::chrono::system_clock::now();
        }

#if not _DEBUG
        // Print the FPS on the OpenCV window
        cv::putText(frame, "FPS: " + std::to_string(FPS), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

        // Display the video frame
        imshow("Real-time VideoCapture", frame);
#endif
        // Wait for 20ms
        waitKey(1);
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

#if not _DEBUG
    
    // Bind the socket
    bind_addr(sock_fd, BROADCAST_PORT);

    // Socket address
    struct sockaddr_in addr_in;

#endif
    // CSI Standard Data Processing Class for processing the CSI data
    size_t received_size = 0;
    CSIStandardDataProcessingClass csiHandeler;

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

            // // Sleep for 1 ms
            // std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
    while (!b_quit) {

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
