//
// Created by Seagosoft on 8/20/2023.
//

#ifndef PC_SAMPLING_SYNCHRONOUSBINPROCESSOR_H
#define PC_SAMPLING_SYNCHRONOUSBINPROCESSOR_H

#include <vector>
#include <chrono>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdint>


#include <opencv2/opencv.hpp>


constexpr int BUF_SIZE          = 1024 * 1024 * 1024;   // buffer size, 1GB
constexpr int FLUSH_INTERVAL    = 60;                   // refresh interval, 60s

struct OpenCVFrameInfo {
    uint32_t    frame_size;
    uint32_t    raw_size;
    uint32_t    timestamp;
    uint32_t    crc32;
    
    uint16_t    width;
    uint16_t    height;
    uint16_t    channels;
    uint16_t    reserved;
};

struct CSIDataFrameInfo {
    uint32_t    frame_size;
    uint32_t    raw_size;
    uint32_t    timestamp;
    uint32_t    crc32;
};


class SynchronousBinProcessor {

private:
    // OpenCV frame
    OpenCVFrameInfo     st_cv_frame;
    CSIDataFrameInfo    st_csi_frame;

    // file stream
    std::ofstream       ofs;

    // vector for cv mat
    uint8_t             *cv_buff; 
    uint8_t             *cv_swap; 

    size_t              cv_buff_size;
    size_t              cv_swap_size;

    // buffer for csi raw data
    uint8_t             *csi_buff; 
    uint8_t             *csi_swap; 

    size_t              csi_buff_size;
    size_t              csi_swap_size;

    // mutex locks.
    std::mutex          mutex_lock;

    // flag for continuing the loop
    bool                b_thread_running;
    bool                b_thread_end;

    // vector for storing original OpenCV frames
    std::vector<cv::Mat> cv_frames;
    std::vector<cv::Mat> cv_frames_swap;

    // VideoWriter for saving video
    cv::VideoWriter     video_writer;

    // backend thread
    std::thread         t_backend_saver;

    // filename handler
    std::string         filename_handler;

    // time recorder for last time flashed
    std::chrono::system_clock::time_point last_updated;

public:

    explicit SynchronousBinProcessor();

    ~SynchronousBinProcessor();

    void open_file();

    void close_file();

    void append_data(cv::Mat& mat);

    void append_data(unsigned char* buf, size_t data_size);

private:

    void save_data_to_bin();

    void swap_buffer();

    void save_data();

};

typedef SynchronousBinProcessor BinProcessor;


#endif //PC_SAMPLING_SYNCHRONOUSBINPROCESSOR_H
