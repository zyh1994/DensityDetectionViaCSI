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

namespace sge {

    class FileStorage {

    private:
        std::string basename;           // 基础文件名
        std::ofstream csv_file;             // 文件流
        std::mutex mutex;               // 互斥锁
        bool isFlushing;                // 是否正在刷新
        std::chrono::time_point<std::chrono::system_clock> lastFlushTime; // 上次刷新时间

    private:
        cv::VideoWriter writer;

    public:
        explicit FileStorage(std::string basename);

        ~FileStorage();

        /**
         * 打开文件
         */
        void open_file();

        /**
         * 关闭文件
         */
        void close_file();

        /**
         * 将缓存中的数据写入文件
         */
        void flush();

        /**
         * 写入数据
         * @param timestamp
         * @param mat
         * @param data
         */
        void write(long long timestamp, cv::Mat& mat,
                   unsigned char* data, size_t size);

    private:

        std::string new_filename();

        bool is_time_to_flush();

        void write_data(unsigned char* data, int size);
    };
}

#endif //PCSAMPLING_FILESTORAGE_H
