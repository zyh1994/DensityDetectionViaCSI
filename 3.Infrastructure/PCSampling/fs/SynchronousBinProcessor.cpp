//
// Created by Seagosoft on 8/20/2023.
//

#include "SynchronousBinProcessor.h"
#include "FileSystemUtils.h"
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
    filename_handler = gen_filename();
    auto filename = filename_handler + ".bin";

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

    // print out the message
    std::cout << "File " << filename_handler << ".bin closed successfully!" << std::endl;
}


void SynchronousBinProcessor::append_data(cv::Mat &mat) {

    // create a new cv mat
    cv::Mat new_mat;

    // resize the cv mat to 320x240, and convert to grayscale
    cv::resize(mat, new_mat, cv::Size(320, 240));
    cv::cvtColor(new_mat, new_mat, cv::COLOR_BGR2GRAY);

    // assign the information to the struct
    OpenCVFrameInfo mat_info{};
    mat_info.frame_size = sizeof(OpenCVFrameInfo) + new_mat.total() * new_mat.elemSize();
    mat_info.raw_size = new_mat.total() * new_mat.elemSize();
    mat_info.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    mat_info.width = new_mat.cols;
    mat_info.height = new_mat.rows;
    mat_info.channels = new_mat.channels();

    // lock the mutex with unique_lock
    std::unique_lock<std::mutex> lock(mutex_lock);

    // copy the data into the buffer
    memcpy(cv_buff + cv_buff_size, &mat_info, sizeof(OpenCVFrameInfo));
    memcpy(cv_buff + cv_buff_size + sizeof(OpenCVFrameInfo), mat.data, mat_info.raw_size);

    // update the buffer size
    cv_buff_size += sizeof(OpenCVFrameInfo) + mat_info.raw_size;

    // now append the original cv frame to the vector
    cv_frames.push_back(mat.clone());
}


void SynchronousBinProcessor::append_data(unsigned char *buf, size_t data_size) {

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

            // swap the cv mat containers
            std::swap(cv_frames, cv_frames_swap);

            // update the last updated time
            last_updated = std::chrono::system_clock::now();

            // reset the count of written data to be zero
            cv_buff_size = csi_buff_size = 0;
        }

        // Then write the data to the file
        {
            // lock the mutex with unique_lock
            std::unique_lock<std::mutex> lock(mutex_lock);

            // open the file
            open_file();

            // check if the file is opened, and write the data to the file
            if (cv_swap_size > 0 && csi_swap_size > 0) {

                // write the size of cv data
                ofs.write(reinterpret_cast<char *>(&cv_swap_size), sizeof(size_t));

                // write the size of csi data
                ofs.write(reinterpret_cast<char *>(&csi_swap_size), sizeof(size_t));

                // write the cv data
                ofs.write(cv_swap, cv_swap_size);

                // write the csi data
                ofs.write(csi_swap, csi_swap_size);
            }

            // save the video
            if (cv_frames_swap.size() > 0) {

                // generate the filename
                auto filename = filename_handler + ".avi";

                // get the FourCC
                auto fourcc = get_fourcc(VideoTypeFourCC::MPEG_4);

                // create a video writer for saving the video
                auto video_writer = VideoHelper::openVideoWriter(
                    filename_handler,
                    fourcc, 
                    30, 
                    cv::Size(1280, 720));

                // check if the video writer is opened
                if (!video_writer.isOpened()) {
                    std::cout << "Error: cannot open video writer!" << std::endl;
                    exit(-1);
                }

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
    }

    // set the flag to be false
    b_thread_end = true;
}
