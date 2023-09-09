#include <csignal>
#include <iostream>
#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>

#include "bin/SynchronousBinProcessor.h"
#include "csi/GeneralCSIDataStruct.h"

using namespace             cv;
#define BUF_SIZE            4096
unsigned char               temp_buf[BUF_SIZE]; // buffer for storing the received data
bool                        b_quit;             // flag for quitting the program
BinProcessor                processor;  // create the synchronous bin processor

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

    // Use image to instead of video capture
    Mat frame = imread("Actresses.jpg");

    // Loop until the user presses the ESC key
    while (!b_quit) {

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

        // Display the video frame
        imshow("Real-time VideoCapture", frame);

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

    // Loop until the user presses the ESC key
    while (!b_quit) {

        // TODO: Generate fake CSI data
//
//        // If the received data is valid
//        if (received_size > 0) {
//            processor.append_data(temp_buf, received_size);
//        }
    }
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

    // This thread will run 1 minute 30 seconds
    std::this_thread::sleep_for(std::chrono::seconds(70));
    b_quit = true;
    
    // Wait all the threads to finish
    video_capture_thread.join();
    csi_sampling_thread.join();

    // Exit the program
    return 0;
}
