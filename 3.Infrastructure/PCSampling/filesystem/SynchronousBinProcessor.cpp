//
// Created by Seagosoft on 8/20/2023.
//

#include "SynchronousBinProcessor.h"
#include "FileSystemUtils.h"


SynchronousBinProcessor::SynchronousBinProcessor() {

    // flag for continuing the loop
    b_thread_running = b_thread_end = false;

    // time recorder for last time flashed
    last_updated = std::chrono::system_clock::now();

    // reset the count of written data to be zero
    cv_buff_size = cv_swap_size = 0;
    csi_buff_size = csi_swap_size = 0;

    // malloc the buffer
    cv_buff = new char[BUF_SIZE];
    cv_swap = new char[BUF_SIZE];

    csi_buff = new char[BUF_SIZE];
    csi_swap = new char[BUF_SIZE];

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

    // free the buffer
    delete[] cv_buff;
    delete[] cv_swap;
    delete[] csi_buff;
    delete[] csi_swap;

    // close the file
    close_file();
}


void SynchronousBinProcessor::open_file() {
    if (ofs.is_open()) {
        ofs.close();
    }

    // generate the filename
    auto filename = gen_filename();

    // open the file
    ofs.open(filename, std::ios::binary | std::ios::out);

    // check if the file is opened
    if (!ofs.is_open()) {
        std::cout << "Error: cannot open file " << filename << std::endl;
        exit(-1);
    } else {
        std::cout << "File " << filename << " opened successfully!" << std::endl;
    }
}


void SynchronousBinProcessor::close_file() {
    if (ofs.is_open()) {
        ofs.close();
    }
}


void SynchronousBinProcessor::append_data(cv::Mat &mat) {

    // assign the information to the struct
    OpenCVFrameInfo mat_info{};
    mat_info.frame_size = sizeof(OpenCVFrameInfo) + mat.total() * mat.elemSize();
    mat_info.raw_size = mat.total() * mat.elemSize();;
    mat_info.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    mat_info.width = mat.cols;
    mat_info.height = mat.rows;
    mat_info.channels = mat.channels();

    // lock the mutex with unique_lock
    std::unique_lock<std::mutex> lock(mutex_lock);

    // copy the data into the buffer
    memcpy(cv_buff + cv_buff_size, &mat_info, sizeof(OpenCVFrameInfo));
    memcpy(cv_buff + cv_buff_size + sizeof(OpenCVFrameInfo), mat.data, mat_info.raw_size);

    // update the buffer size
    cv_buff_size += sizeof(OpenCVFrameInfo) + mat_info.raw_size;
}


void SynchronousBinProcessor::append_data(char *buf, size_t data_size) {

    // assign the information to the struct
    CSIDataFrameInfo csi_info{};
    csi_info.frame_size = sizeof(CSIDataFrameInfo) + data_size;
    csi_info.raw_size = data_size;
    csi_info.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    // lock the mutex with unique_lock
    std::unique_lock<std::mutex> lock(mutex_lock);

    // copy the data into the buffer
    memcpy(csi_buff + csi_buff_size, &csi_info, sizeof(CSIDataFrameInfo));
    memcpy(csi_buff + csi_buff_size + sizeof(CSIDataFrameInfo), buf, data_size);

    // update the buffer size
    csi_buff_size += sizeof(CSIDataFrameInfo) + data_size;
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
            // lock the mutex with unique_lock
            std::unique_lock<std::mutex> lock(mutex_lock);

            // swap the buffers
            std::swap(cv_buff, cv_swap);
            std::swap(cv_buff_size, cv_swap_size);

            // swap the cv mat containers
            std::swap(csi_buff, csi_swap);
            std::swap(csi_buff_size, csi_swap_size);

            // update the last updated time
            last_updated = std::chrono::system_clock::now();

            // reset the count of written data to be zero
            cv_buff_size = csi_buff_size = 0;
        }

        // Then write the data to the file
        {
            // TODO: write the data to the file


        }

    }

    // set the flag to be false
    b_thread_end = true;
}
