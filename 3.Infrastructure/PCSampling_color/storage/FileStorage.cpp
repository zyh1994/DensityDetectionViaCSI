//
// Created by Orlando Chen on 2023/07/02.
//

#include "FileStorage.h"

namespace sge {

    FileStorage::FileStorage(std::string basename) :
        basename(std::move(basename)),
        isFlushing(false) {
        lastFlushTime = std::chrono::system_clock::now();
    }

    FileStorage::~FileStorage() {
        close_file();
    }

    void FileStorage::open_file() {
        if (csv_file.is_open()) {
            csv_file.close();
        }

        // 生成新的文件名
        auto filename = new_filename();

        // 创建新的文件
        csv_file.open(filename, std::ios::binary | std::ios::app);
    }

    void FileStorage::close_file() {
        if (csv_file.is_open()) {
            csv_file.close();
        }

        if (!isFlushing) {
            flush();
        }
    }

    void FileStorage::flush() {

        if (isFlushing) {
            return; // 如果已经在刷新，则不执行操作
        }

        // 设置刷新标志
        isFlushing = true;

        // 将交换后的缓存写入文件
//        auto flushThread = std::thread(&FileStorage::write_data, this, temp_buf_swap, write_size);

//        // 重置数据
//        buf_left_size = BUF_SIZE;
//        write_size = 0;

        // 分离线程
//        flushThread.detach();
    }


    void FileStorage::write(long long timestamp, cv::Mat& mat,
               unsigned char* data, size_t size) {

    }

//    void FileStorage::write(const unsigned char* data, size_t size) {
//        std::lock_guard<std::mutex> lock(mutex);
//        if (size > buf_left_size || is_time_to_flush()) {
//            flush();
//        }
//
//        // 将数据拷贝到缓存中
//        std::memcpy(temp_buf + (BUF_SIZE - buf_left_size), data, size);
//        buf_left_size -= size;
//        write_size += size;
//    }


    std::string FileStorage::new_filename() {
        // 获取当前时间
        std::time_t now = std::time(nullptr);

        // 将时间转换为本地时间结构
        std::tm* localTime = std::localtime(&now);

        // 创建一个字符串流对象
        std::stringstream ss;

        // 格式化时间为指定格式，并将其写入字符串流
        ss << basename << "_"
        //    << std::put_time(localTime, "%Y%m%d%H%M%S")  // We don't need seconds here
            << std::put_time(localTime, "%Y%m%d%H%M");

        // 从字符串流中获取格式化后的时间字符串
        std::string formattedTime = ss.str();

        // 返回格式化后的文件名
        return formattedTime;
    }

    bool FileStorage::is_time_to_flush() {
        auto currentTime = std::chrono::system_clock::now();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastFlushTime).count();
        return elapsedSeconds >= FLUSH_INTERVAL;
    }

    void FileStorage::write_data(unsigned char* data, int size) {
        open_file();
        mutex.lock();
        if (csv_file.is_open()) {
            csv_file.write(reinterpret_cast<char*>(data), size);

        }
        mutex.unlock();
        close_file();

    //        std::cout << "flush " << size << " bytes" << std::endl;

        // 更新刷新时间和刷新标志
        lastFlushTime = std::chrono::system_clock::now();
        isFlushing = false;
    }
}
