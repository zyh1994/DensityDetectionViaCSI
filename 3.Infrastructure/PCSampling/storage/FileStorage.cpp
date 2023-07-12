//
// Created by Orlando Chen on 2023/07/02.
//

#include "FileStorage.h"

#include "../cv/VideoHelper.h"

#include <cstdlib>


namespace sge {

    std::string generate_new_filename() {
        // 获取当前时间
        std::time_t now = std::time(nullptr);

        // 将时间转换为本地时间结构
        std::tm *localTime = std::localtime(&now);

        // 创建一个字符串流对象
        std::stringstream ss;

        // 格式化时间为指定格式，并将其写入字符串流
        ss << std::put_time(localTime, "%Y%m%d%H%M");

        // 从字符串流中获取格式化后的时间字符串
        std::string formattedTime = ss.str();

        // 返回格式化后的文件名
        return formattedTime;
    }

    size_t generate_data_for_bin_file(
            u_int64_t timestamp,
            u_int8_t* thumbnail_data,
            u_int64_t thumbnail_size,
            u_int8_t* csi_raw_data,
            u_int64_t csi_raw_size,
            u_int8_t** bin_frame_out,
            size_t* bin_frame_size_available_out){

        // Calculate the size of the bin frame
        size_t frame_size =
                sizeof(u_int64_t)           // total size
                + sizeof(thumbnail_size)    // thumbnail size
                + sizeof(csi_raw_size)      // csi data size
                + sizeof(timestamp)         // the size of timestamp
                + thumbnail_size            // the size of thumbnail
                + csi_raw_size              // the size of csi raw data
                + sizeof(u_int8_t) * 2;     // \r\n

        // Allocate memory for the bin frame
        // if it is not allocated or the size is not enough
        if (*bin_frame_out == nullptr || frame_size > *bin_frame_size_available_out) {

            // Delete the old memory
            if (*bin_frame_out != nullptr) {
                delete[] *bin_frame_out;
            }

            // Allocate new memory
            *bin_frame_out = new u_int8_t[frame_size];

            // Update the size of the bin frame
            *bin_frame_size_available_out = frame_size;

            std::cout << "Allocated new memory for bin frame, size: " << frame_size << std::endl;
        }

        // Assemble the bin frame
        size_t offset = 0;
        memcpy(*bin_frame_out + offset, &frame_size, sizeof(frame_size));
        offset += sizeof(frame_size);

//        std::cout << "Copied frame size: " << frame_size << std::endl;

        memcpy(*bin_frame_out + offset, &thumbnail_size, sizeof(thumbnail_size));
        offset += sizeof(thumbnail_size);

//        std::cout << "Copied thumbnail size: " << thumbnail_size << std::endl;

        memcpy(*bin_frame_out + offset, &csi_raw_size, sizeof(csi_raw_size));
        offset += sizeof(csi_raw_size);

//        std::cout << "Copied csi raw size: " << csi_raw_size << std::endl;

        memcpy(*bin_frame_out + offset, &timestamp, sizeof(timestamp));
        offset += sizeof(timestamp);

//        std::cout << "Copied timestamp: " << timestamp << std::endl;

        memcpy(*bin_frame_out + offset, thumbnail_data, thumbnail_size);
        offset += thumbnail_size;

//        std::cout << "Copied thumbnail data..." << std::endl;

        memcpy(*bin_frame_out + offset, csi_raw_data, csi_raw_size);
        offset += csi_raw_size;

//        std::cout << "Copied csi raw data..." << std::endl;

        memcpy(*bin_frame_out + offset, "\r\n", sizeof(u_int8_t) * 2);

//        std::cout << "Copied \\r\\n" << std::endl;

        return frame_size;
    }

    FileStorage::FileStorage() {

        // Update the last timestamp
        last_updated = std::chrono::system_clock::now();

        // Temporary buffer for the bin file
        temp_buf = new unsigned char[BUF_SIZE];

        // Temporary buffer for swapping the buffer
        temp_buf_swap = new unsigned char[BUF_SIZE];

        // Temporary bin frame buffer
        bin_frame_buf = nullptr;

        // The size of bin frame
        bin_frame_size = 0;

        // Whether it is flushing
        isFlushing = false;

        // The size of data written to the file
        write_size = 0;

        // The size of the left buffer
        buf_left_size = BUF_SIZE;

        // Open the files
//        open_files();
    }

    FileStorage::~FileStorage() {
        close_files();

        delete[] temp_buf;
        delete[] temp_buf_swap;
        delete[] bin_frame_buf;

        std::cout << "FileStorage destroyed." << std::endl;
    }

    void FileStorage::open_files() {
        if (bin_file.is_open()) {
            bin_file.close();
        }

        // 生成新的文件名
        auto base_name = generate_new_filename();

        // 创建新的文件
        bin_file.open(base_name + ".bin",
                      std::ios::binary | std::ios::app);
        std::cout << "Opened file: " << base_name << ".bin" << std::endl;

        // Create a new video writer
        writer = VideoHelper::openVideoWriter(base_name + ".avi",
                                              get_fourcc(VideoTypeFourCC::MPEG_4),
                                              30, cv::Size(1280, 720));
        std::cout << "Opened file: " << base_name << ".avi" << std::endl;
    }

    void FileStorage::close_files() {
        if (bin_file.is_open()) {
            bin_file.close();
        }

        if (writer.isOpened()) {
            writer.release();
        }

        std::cout << "Closed files." << std::endl;
    }

    void FileStorage::write(long long timestamp, cv::Mat &mat,
                            unsigned char *data, ssize_t size) {

        if (size > buf_left_size || is_time_up_to_flush()) {
//            std::cout << "Flushing..." << std::endl;
            flush();
        }

        // Copy a new video frame and put it into the video vector
        video_frames.push_back(mat.clone());

        // Compress the 720p image to 320x180 grayscale image
//        std::cout << "Compressing..." << std::endl;
        cv::Mat compressedMat;
        cv::resize(mat, compressedMat, cv::Size(320, 180));
        cv::cvtColor(compressedMat, compressedMat, cv::COLOR_BGR2GRAY);

        // Get the bin frame data by passing the raw data
//        std::cout << "Generating bin frame..." << std::endl;
        size_t updated_size = generate_data_for_bin_file(
                timestamp,
                compressedMat.data,
                compressedMat.total() * compressedMat.elemSize(),
                data,
                size,
                &bin_frame_buf,
                &bin_frame_size);

        // Convert the bin frame size to int
        int frame_size = static_cast<int>(updated_size);

        // Copy the bin frame data to the buffer
//        std::cout << "Copying bin frame data to the buffer..." << std::endl;
        std::memcpy(temp_buf + (BUF_SIZE - buf_left_size),
                    bin_frame_buf,
                    bin_frame_size);
        buf_left_size -= frame_size;
        write_size += frame_size;
    }

    bool FileStorage::is_time_up_to_flush() {
        auto currentTime = std::chrono::system_clock::now();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - last_updated).count();
        return elapsedSeconds >= FLUSH_INTERVAL;
    }

    void FileStorage::write_data_by_calling_thread(
            unsigned char *data, int size) {

        // Write the data to the file
        open_files();
        mutex.lock();

        // Write the bin file
        if (bin_file.is_open()) {
            bin_file.write(reinterpret_cast<char*>(data), size);
        }

        // Write the video frames to the video file
        for (auto &frame : video_frames_swap) {
            writer.write(frame);
        }

        // Clear the video frames
        video_frames_swap.clear();

        mutex.unlock();
        close_files();

        // Update the last timestamp
        last_updated = std::chrono::system_clock::now();
        isFlushing = false;
    }

    void FileStorage::flush() {
        // If the buffer is already being flushed, do nothing
        if (isFlushing) {
            std::cout << "Buffer is already being flushed." << std::endl;
            return;
        }

        // If the buffer is not being used, do nothing
        if (write_size == 0) {
            std::cout << "Buffer is not being used." << std::endl;
            return;
        }

        // Update the flag
        isFlushing = true;

        // Swap the buffers
        std::swap(temp_buf, temp_buf_swap);

        // Swap the vector of video frames
        std::swap(video_frames, video_frames_swap);

        // We can write the data to the file
        auto flushThread = std::thread(&FileStorage::write_data_by_calling_thread,
                                       this,
                                       temp_buf_swap,
                                       write_size);

        // Reset the buffer
        buf_left_size = BUF_SIZE;
        write_size = 0;

        // Detach the thread
        flushThread.detach();
    }
}
