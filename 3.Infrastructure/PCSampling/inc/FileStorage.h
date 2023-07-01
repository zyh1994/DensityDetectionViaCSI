//
// Created by Seagosoft on 7/1/2023.
//

#ifndef PCSAMPLING_FILESTORAGE_H
#define PCSAMPLING_FILESTORAGE_H

#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <thread>

#define BIN_FILENAME        "csi_videoc"
#define BIN_SUFFIX          ".bin"
#define BUF_SIZE            1024 * 1024 * 100 // 100 MB

class FileStorage {
public:
    FileStorage();
    ~FileStorage();

    /**
     * @brief Open the file
     */
    void open_file();

    /**
     * @brief Close the file
     */
    void close_file();

    /**
     * @brief Write the data to the file
     * @param data
     * @param size
     */
    void write_data(unsigned char* data, size_t size);

    /**
     * @brief Get the available size of the buffer
     * @return
     */
    size_t get_available_size();

    /**
     * @brief Get the buffer size
     * @return
     */
    size_t get_buf_size();

    /**
     * @brief Flush the buffer
     */
    void flush();

private:
    std::ofstream file;

    // current filename
    std::string filename;

    // last updated time
    long file_last_time;

    // file count
    size_t file_count;

    // temp buffer for storing the data
    unsigned char* temp_buf;

    // temp buffer for swapping the data
    unsigned char* temp_buf_swap;

    // left buffer size
    size_t buf_left_size;

    // Get the current time in string
    std::string get_time();

    // Get the current time in long
    long get_time_long();

    // Generate the filename with the current time
    std::string generate_filename();

private:
    // thread for flushing the buffer
    std::thread flush_thread;

    // flag for flushing the buffer
    bool flush_flag;

    // size of the data to be written
    size_t write_size;

    // flush the buffer
    void flush_buffer();
};

#endif //PCSAMPLING_FILESTORAGE_H
