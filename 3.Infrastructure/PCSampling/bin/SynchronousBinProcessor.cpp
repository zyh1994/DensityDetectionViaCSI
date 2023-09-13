//
// Created by Seagosoft on 8/20/2023.
//

#include "GeneralUtils.h"
#include "SynchronousBinProcessor.h"
#include "../cv/VideoHelper.h"
#include "../csi/StandardCSIDataStruct.h"


#include <cstdio>
#include <fstream>

SynchronousBinProcessor::SynchronousBinProcessor() {

    // flag for continuing the loop
    b_thread_running = b_thread_end = false;

    // time recorder for last time flashed
    last_updated = std::chrono::system_clock::now();

    // reset the count of written data to be zero
    cv_buff_size = cv_swap_size = 0;
    csi_buff_size = csi_swap_size = 0;

    // malloc the buffer
    ptr_cv_buff = new uint8_t[BUF_SIZE];
    ptr_cv_swap = new uint8_t[BUF_SIZE];

    ptr_csi_buff = new uint8_t[BUF_SIZE];
    ptr_csi_swap = new uint8_t[BUF_SIZE];

    // start the backend thread
    t_backend_saver = std::thread(&SynchronousBinProcessor::save_to_bin, this);

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
    delete[] ptr_cv_buff;
    delete[] ptr_cv_swap;
    delete[] ptr_csi_buff;
    delete[] ptr_csi_swap;

    // close the file
    close_file();
}


void SynchronousBinProcessor::open_file() {
    if (ofs.is_open()) {
        ofs.close();
    }

    // generate the filename
    filename_handler = gen_filename();
    auto bin_file = filename_handler + ".bin";

    // open the file
    ofs.open(bin_file, std::ios::binary | std::ios::out);

    // check if the file is opened
    if (!ofs.is_open()) {
        std::cout << "Error: cannot open file " << bin_file << std::endl;
        exit(-1);
    } else {
        std::cout << "File " << filename_handler << ".bin opened successfully!" << std::endl;
    }


    // create a new video writer
    video_writer = VideoHelper::openVideoWriter(
            filename_handler + ".avi",
            get_fourcc(VideoTypeFourCC::MPEG_4),
            15, // <--- target device is only 15 FPS available, so the regenerated video should keep in 15 FPS
            cv::Size(1280, 720));

    // check if the video writer is opened
    if (!video_writer.isOpened()) {
        std::cout << "Error: cannot open video writer!" << std::endl;
        exit(-1);
    } else {
        std::cout << "File " << filename_handler << ".avi opened successfully!" << std::endl;
    }
}


void SynchronousBinProcessor::close_file() {
    if (ofs.is_open()) {
        ofs.close();
    }

    if (video_writer.isOpened()) {
        video_writer.release();
    }

    // print out the message
    std::cout << "File " << filename_handler << ".bin closed successfully!" << std::endl;

    // print out the message
    std::cout << "File " << filename_handler << ".avi closed successfully!" << std::endl;
}


void SynchronousBinProcessor::append_data(cv::Mat &mat) {

    // create a new cv mat
    cv::Mat mat_320p;

    // resize the cv mat to 320p
    cv::resize(mat, mat_320p, cv::Size(320, 180));
    cv::cvtColor(mat_320p, mat_320p, cv::COLOR_BGR2GRAY);

#if _DEBUG
    // show the resized image
    cv::imshow("Grayscale VideoCapture", mat_320p);
#endif

    // Assign the timestamp
    st_cv_frame.timestamp = timestamp();

    // Calculate the crc
    auto frame_len = mat_320p.cols * mat_320p.rows * mat_320p.channels();
    st_cv_frame.crc32 = calculate_crc32(mat_320p.data, frame_len);

    // Assign the width
    st_cv_frame.width = mat_320p.cols;

    // Assign the height
    st_cv_frame.height = mat_320p.rows;

    // Assign the channels
    st_cv_frame.channels = mat_320p.channels();

    // copy the data into the struct
    memcpy(st_cv_frame.bytes, mat_320p.data, frame_len);

    // lock the mutex with unique_lock
    std::unique_lock<std::mutex> lock(mutex_lock);

    // copy the data into the buffer
    memcpy(ptr_cv_buff + cv_buff_size, &st_cv_frame, CVDATA_CHECKSUM_SIZE);

    // update the buffer size
    cv_buff_size += CVDATA_CHECKSUM_SIZE;

    // push the original frame to the vector
    cv_frames.push_back(mat.clone());
}


void SynchronousBinProcessor::append_data(unsigned char *buf, size_t data_size) {

    // assign the crc32
    st_csi_frame.crc32 = calculate_crc32(buf, data_size);

    // copy the data into the struct
    memcpy(st_csi_frame.bytes, buf, data_size);

    // lock the mutex with unique_lock
    std::unique_lock<std::mutex> lock(mutex_lock);

    // copy the data into the buffer
    memcpy(ptr_csi_buff + csi_buff_size, &st_csi_frame, CSISTD_CHECKSUM_SIZE);

    // update the buffer size
    csi_buff_size += CSISTD_CHECKSUM_SIZE;

    // print out the message
    std::cout << "CSI data size " << csi_buff_size << std::endl;
}


void SynchronousBinProcessor::swap_buffer() {
    // lock the mutex with unique_lock
    std::unique_lock<std::mutex> lock(mutex_lock);

    // swap the buffers
    std::swap(ptr_cv_buff, ptr_cv_swap);
    std::swap(cv_buff_size, cv_swap_size);

    // swap the cv mat containers
    std::swap(ptr_csi_buff, ptr_csi_swap);
    std::swap(csi_buff_size, csi_swap_size);

    // swap the cv mat containers
    std::swap(cv_frames, cv_frames_swap);

    // update the last updated time
    last_updated = std::chrono::system_clock::now();

    // reset the count of written data to be zero
    cv_buff_size = csi_buff_size = 0;

    // print out the message
    std::cout << "Buffer swapped!" << std::endl;
};


void SynchronousBinProcessor::save_data() {

    // report the memory usage first
    auto cv_data_usage = cv_swap_size / 1024 / 1024;
    auto csi_data_usage = csi_swap_size / 1024 / 1024;

    std::cout << "CV memory usage " << cv_data_usage << " MB" << std::endl
                << "CSI memory usage " << csi_data_usage << " MB" << std::endl;

    // open the file
    open_file();

    // check if the file is opened, and write the data to the file
    if (cv_swap_size > 0 && csi_swap_size > 0) {

        // write the size of cv data
        ofs.write(reinterpret_cast<char *>(&cv_swap_size), sizeof(size_t));

        // write the size of csi data
        ofs.write(reinterpret_cast<char *>(&csi_swap_size), sizeof(size_t));

        // write the cv data
        ofs.write(reinterpret_cast<char *>(ptr_cv_swap), static_cast<long>(cv_swap_size));

        // write the csi data
        ofs.write(reinterpret_cast<char *>(ptr_csi_swap), static_cast<long>(csi_swap_size));
    }

    // print out the message
    std::cout << "Data saved!" << std::endl;

    // save the video
    if (!cv_frames_swap.empty()) {

        // write the frames to the video
        for (auto &frame : cv_frames_swap) {
            video_writer.write(frame);
        }

        // clear the vector
        cv_frames_swap.clear();
    }

    // print out the message
    std::cout << "Video saved!" << std::endl;

    // close the file
    close_file();
}


void SynchronousBinProcessor::save_to_bin(){

    // set the flag to be true
    b_thread_running = true;

    // loop for the backend thread
    while (b_thread_running) {

        // sleep for 1 minute
        std::this_thread::sleep_for(std::chrono::minutes(1));

        // Print out the message
        std::cout << "Flushing the buffer..." << std::endl;

        // Use the mutex to swap the buffer
        swap_buffer();

        // Then write the data to the file
        save_data();
    }

    // set the flag to be false
    b_thread_end = true;
}
