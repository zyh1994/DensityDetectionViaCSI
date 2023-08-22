//
// Created by Seagosoft on 8/20/2023.
//

#ifndef PC_SAMPLING_SYNCHRONOUSBINPROCESSOR_H
#define PC_SAMPLING_SYNCHRONOUSBINPROCESSOR_H

#include <vector>
#include <chrono>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <opencv2/opencv.hpp>


constexpr int BUF_SIZE = 1024 * 1024 * 1024; // 缓存大小，单位为字节
constexpr int FLUSH_INTERVAL = 60;           // 刷新间隔，单位为秒

class SynchronousBinProcessor {

private:
    // vector for cv mat
    std::vector<cv::Mat> frames_container, frames_swap;

    // buffer for csi raw data
    char* bin_buf_main;
    char* bin_buf_swap;

    // mutex when swapping the buffer
    std::mutex mutex_lock;

    // flag for continuing the loop
    bool b_thread_running;
    bool b_thread_end;

    // time recorder for last time flashed
    std::chrono::system_clock::time_point last_updated;

    // 写入文件的数据大小
    size_t buf_data_size;
    size_t buf_data_swap;

    // backend thread
    std::thread t_backend_saver;

    // thread condition variable
//    std::condition_variable cv;
public:

    SynchronousBinProcessor();

    ~SynchronousBinProcessor();

    void open_file();

    void close_file();

    void append_data(long long timestamp, cv::Mat& mat);

    void append_data(long long timestamp, char* buf, size_t data_size);

private:
    void save_data_to_bin();
};


#endif //PC_SAMPLING_SYNCHRONOUSBINPROCESSOR_H
