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

#include <opencv2/opencv.hpp>


constexpr int BUF_SIZE = 1024 * 1024 * 1024; // buffer size, 1GB
constexpr int FLUSH_INTERVAL = 60;           // refresh interval, 60s

struct OpenCVFrameInfo {
    size_t frame_size;
    size_t raw_size;
    long long timestamp;
    int width;
    int height;
    int channels;
};

struct CSIDataFrameInfo {
    size_t frame_size;
    size_t raw_size;
    long long timestamp;
};


class SynchronousBinProcessor {

private:
    // file stream
    std::ofstream ofs;

    // vector for cv mat
    char* cv_buff; 
    char* cv_swap; 

    size_t cv_buff_size;
    size_t cv_swap_size;

    // buffer for csi raw data
    char* csi_buff; 
    char* csi_swap; 

    size_t csi_buff_size;
    size_t csi_swap_size;

    // mutex when swapping the buffer
    std::mutex mutex_lock;

    // flag for continuing the loop
    bool b_thread_running;
    bool b_thread_end;

    // vector for storing original OpenCV frames
    std::vector<cv::Mat> cv_frames;
    std::vector<cv::Mat> cv_frames_swap;

    // VideoWriter for saving video
    cv::VideoWriter video_writer;

    // time recorder for last time flashed
    std::chrono::system_clock::time_point last_updated;

    // backend thread
    std::thread t_backend_saver;

    // filename handler
    std::string filename_handler;

public:

    SynchronousBinProcessor();

    ~SynchronousBinProcessor();

    void open_file();

    void close_file();

    void append_data(cv::Mat& mat);

    void append_data(unsigned char* buf, size_t data_size);

private:
    void save_data_to_bin();
};


#endif //PC_SAMPLING_SYNCHRONOUSBINPROCESSOR_H
