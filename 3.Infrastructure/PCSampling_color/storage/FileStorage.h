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

constexpr int BUF_SIZE = 512 * 1024 * 1024; // 缓存大小，单位为字节
constexpr int FLUSH_INTERVAL = 60; // 刷新间隔，单位为秒
constexpr std::string FILE_EXT = ".csv"; // 文件扩展名

namespace sge {

    class FileStorage {

    private:
        std::string basename; // 基础文件名
        std::ofstream file; // 文件流
        unsigned char* temp_buf; // 缓存数据
        unsigned char* temp_buf_swap; // 交换缓存
        std::mutex mutex; // 互斥锁
        bool isFlushing; // 是否正在刷新
        std::chrono::time_point<std::chrono::system_clock> lastFlushTime; // 上次刷新时间
        int write_size; // 写入文件的数据大小
        int buf_left_size; // 缓存剩余空间大小
        std::vector<std::string> filenames; // 文件名列表

    public:
        explicit FileStorage(std::string basename);

        ~FileStorage();

        void open_file();

        void close_file();

        inline std::vector<std::string> get_filenames() { return filenames; }

        void flush();

        void write(const unsigned char* data, size_t size);

        void write(const std::string& data);

    private:

        std::string new_filename();

        bool is_time_to_flush();

        void write_data(unsigned char* data, int size);
    };
}

#endif //PCSAMPLING_FILESTORAGE_H
