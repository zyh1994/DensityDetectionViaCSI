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

//constexpr int BUF_SIZE = 512 * 1024 * 1024; // 缓存大小，单位为字节
constexpr int FLUSH_INTERVAL = 60;          // 刷新间隔，单位为秒

namespace sge {

    class FileStorage {

    private:
        // std::string basename;           // 基础文件名
        std::ofstream csv_file;         // 文件流

        std::chrono::system_clock::time_point last_updated; // 上次刷新时间

    private:
        // Use the VideoWriter class object to record the video
        cv::VideoWriter writer;

    public:
        // explicit FileStorage(std::string basename);
        explicit FileStorage();

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
         * 写入数据
         * @param timestamp
         * @param mat
         * @param data
         */
        void write(long long timestamp, cv::Mat& mat,
                   unsigned char* data, size_t size);

    private:

        /**
         * 判断是否需要刷新
         * @return
         */
        bool is_time_to_flush();

        /**
         * 生成新的文件名
         * @return
         */
        std::string new_filename();
    };
}

#endif //PCSAMPLING_FILESTORAGE_H
