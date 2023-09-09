//
// Created by Seagosoft on 8/20/2023.
//

#include "SynchronousBinProcessor.h"
#include "GeneralUtils.h"
#include "../cv/VideoHelper.h"


SynchronousBinProcessor::SynchronousBinProcessor() {

    // flag for continuing the loop
    b_thread_running = b_thread_end = false;

    // time recorder for last time flashed
    last_updated = std::chrono::system_clock::now();

    // reset the count of written data to be zero
    cv_buff_size = cv_swap_size = 0;
    csi_buff_size = csi_swap_size = 0;

    // malloc the buffer
    cv_buff = new uint8_t[BUF_SIZE];
    cv_swap = new uint8_t[BUF_SIZE];

    csi_buff = new uint8_t[BUF_SIZE];
    csi_swap = new uint8_t[BUF_SIZE];

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
            30,
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

    // get the size of OpenCvFrameInfo
    auto struct_len = sizeof(struct OpenCVFrameInfo);

    // assign the length of the raw data
    st_cv_frame.raw_size = mat_320p.total() * mat_320p.elemSize();

    // calculate the entire frame size
    st_cv_frame.frame_size = struct_len + st_cv_frame.raw_size;

    // assign the timestamp
    st_cv_frame.timestamp = timestamp();

    // assign the width and height
    st_cv_frame.width = mat_320p.cols;
    st_cv_frame.height = mat_320p.rows;

    // assign the channels
    st_cv_frame.channels = mat_320p.channels();

    // calculate the crc
    st_cv_frame.crc32 = calculate_crc32(mat_320p.data, st_cv_frame.raw_size);

    // lock the mutex with unique_lock
    std::unique_lock<std::mutex> lock(mutex_lock);

    // copy the data into the buffer
    memcpy(cv_buff + cv_buff_size, &st_cv_frame, struct_len);
    memcpy(cv_buff + cv_buff_size + struct_len, mat.data, st_cv_frame.raw_size);

    // update the buffer size
    cv_buff_size += struct_len + st_cv_frame.raw_size;

    // push the frame to the vector
    cv_frames.push_back(mat.clone());
}


void SynchronousBinProcessor::append_data(unsigned char *buf, size_t data_size) {

    // get the size of CSIDataFrameInfo
    auto struct_len = sizeof(struct CSIDataFrameInfo);

    // assign the information to the struct
    st_csi_frame.frame_size = struct_len + data_size;

    // assign the length of the raw data
    st_csi_frame.raw_size = data_size;

    // assign the timestamp
    st_csi_frame.timestamp = timestamp();

    // assign the crc32
    st_csi_frame.crc32 = calculate_crc32(buf, data_size);

    // lock the mutex with unique_lock
    std::unique_lock<std::mutex> lock(mutex_lock);

    // copy the data into the buffer
    memcpy(csi_buff + csi_buff_size, &st_csi_frame, struct_len);
    memcpy(csi_buff + csi_buff_size + struct_len, buf, data_size);

    // update the buffer size
    csi_buff_size += struct_len + data_size;
}


void SynchronousBinProcessor::swap_buffer() {
    // lock the mutex with unique_lock
    std::unique_lock<std::mutex> lock(mutex_lock);

    // swap the buffers
    std::swap(cv_buff, cv_swap);
    std::swap(cv_buff_size, cv_swap_size);

    // swap the cv mat containers
    std::swap(csi_buff, csi_swap);
    std::swap(csi_buff_size, csi_swap_size);

    // swap the cv mat containers
    std::swap(cv_frames, cv_frames_swap);

    // update the last updated time
    last_updated = std::chrono::system_clock::now();

    // reset the count of written data to be zero
    cv_buff_size = csi_buff_size = 0;
};


void SynchronousBinProcessor::save_data() {

    // report the memory usage first
    auto cv_data_usage = (double)cv_swap_size / (1024 * 1024);
    auto csi_data_usage = (double)csi_swap_size / (1024 * 1024);

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
        ofs.write(reinterpret_cast<char *>(cv_swap), static_cast<long>(cv_swap_size));

        // write the csi data
        ofs.write(reinterpret_cast<char *>(csi_swap), static_cast<long>(csi_swap_size));
    }

    // save the video
    if (!cv_frames_swap.empty()) {

        // write the frames to the video
        for (auto &frame : cv_frames_swap) {
            video_writer.write(frame);
        }

        // clear the vector
        cv_frames_swap.clear();
    }

    // close the file
    close_file();
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
        swap_buffer();

        // Then write the data to the file
        save_data();
    }

    // set the flag to be false
    b_thread_end = true;
}
