//
// Created by Seagosoft on 8/20/2023.
//

#include "SynchronousBinProcessor.h"

SynchronousBinProcessor::SynchronousBinProcessor() {

    // buffer for csi raw data
    bin_buf_main = new char[BUF_SIZE];
    bin_buf_swap = new char[BUF_SIZE];

    // flag for continuing the loop
    b_thread_running = b_thread_end = false;

    // time recorder for last time flashed
    last_updated = std::chrono::system_clock::now();

    // reset the count of written data to be zero
    buf_data_swap = buf_data_size = 0;

    // start the backend thread
    t_backend_saver = std::thread(&SynchronousBinProcessor::save_data_to_bin, this);

    // detach the thread
    t_backend_saver.detach();
}

SynchronousBinProcessor::~SynchronousBinProcessor() {

    // set the flag to be false
    b_thread_running = false;

    // wait for the thread to end
    while (!b_thread_end) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // release the memory
    delete bin_buf_main;
    delete bin_buf_swap;

    // close the file
    close_file();
}

void SynchronousBinProcessor::open_file() {

}

void SynchronousBinProcessor::close_file() {

}

void SynchronousBinProcessor::append_data(long long int timestamp, cv::Mat &mat) {

}

void SynchronousBinProcessor::append_data(long long int timestamp, char *buf, size_t data_size) {

}

void SynchronousBinProcessor::save_data_to_bin(){

    // set the flag to be true
    b_thread_running = true;

    // loop for the backend thread
    while (b_thread_running) {

        // The following loop is for time interval control
        // if the time interval is not reached, the loop will continue
        {
            // get current time clock
            auto current_time = std::chrono::system_clock::now();

            // calculate the elapsed time
            auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_updated).count();

            // if a minute does not pass
            if (elapsed_time < FLUSH_INTERVAL) {
                // sleep for a millisecond
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

                // continue the loop
                continue;
            }
        }

        // Use the mutex to swap the buffer
        {
            // lock the mutex
            std::unique_lock<std::mutex> lock(mutex_lock);

            // swap the buffers
            std::swap(bin_buf_main, bin_buf_swap);

            // swap the data size
            std::swap(buf_data_size, buf_data_swap);

            // swap the cv mat containers
            std::swap(frames_container, frames_swap);

            // update the last updated time
            last_updated = std::chrono::system_clock::now();

            // unlock the mutex
        }

        // Then write the data to the file
        {
            // TODO: write the data to the file
        }

    }

    // set the flag to be false
    b_thread_end = true;
}
