//
// Created by Seagosoft on 7/1/2023.
//

#ifndef PCSAMPLING_FILESTORAGE_H
#define PCSAMPLING_FILESTORAGE_H

#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <thread>
#include <mutex>
#include <utility>
#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>

#include <opencv2/opencv.hpp>

constexpr int BUF_SIZE = 512 * 1024 * 1024; // 缓存大小，单位为字节
constexpr int FLUSH_INTERVAL = 60;          // 刷新间隔，单位为秒

// namespace sge {

    class FileStorage {

    private:
        // 文件流
        std::ofstream bin_file;

        // 缓存数据
        unsigned char* temp_buf;
        // 交换缓存
        unsigned char* temp_buf_swap;
        // temporary bin frame buffer
        unsigned char* bin_frame_buf;
        // 互斥锁
        std::mutex mutex; 
        // 是否正在刷新
        bool isFlushing;
        // 上次刷新时间
        std::chrono::system_clock::time_point last_updated;
        // 写入文件的数据大小
        int write_size;
        // 缓存剩余空间大小
        int buf_left_size;
        // 二进制帧大小
        size_t bin_frame_size;

        // Use a vector to store the video frames
        std::vector<cv::Mat> video_frames;
        std::vector<cv::Mat> video_frames_swap;

    private:
        // Use the VideoWriter class object to record the video
        cv::VideoWriter writer;

    public:
        explicit FileStorage();

        ~FileStorage();

        /**
         * 打开文件
         */
        void open_files();

        /**
         * 关闭文件
         */
        void close_files();

        /**
         * Write the data to the file
         * @param timestamp
         * @param mat
         * @param data
         * @param size
         */
        void write(long long timestamp, cv::Mat& mat,
                   unsigned char* data, ssize_t size);

        /**
         * Flush the data to the file.
         * This will be called when the time interval is reached
         * Or the buffer is going to be full.
         *
         * It will call a thread to write the data to the file (write_data_by_calling_thread)
         */
        void flush();

    private:

        /**
         * When time is up, flush the data to the file
         * @return
         */
        bool is_time_up_to_flush();

        /**
         * Use the thread to write the data to the file
         * @param data
         * @param size
         */
        void write_data_by_calling_thread(unsigned char* data,
                                          int size);
    };
// }

#endif //PCSAMPLING_FILESTORAGE_H
