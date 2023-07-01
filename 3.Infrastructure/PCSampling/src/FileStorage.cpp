//
// Created by Seagosoft on 7/1/2023.
//

#include "../inc/FileStorage.h"


FileStorage::FileStorage()
{
    // Initialize the parameters
    file_count = 0;
    file_last_time = 0;

    // Allocate the buffer
    buf_left_size = BUF_SIZE;

    // Allocate the buffer
    temp_buf = new unsigned char[BUF_SIZE];
    temp_buf_swap = new unsigned char[BUF_SIZE];

    // Open the file
    open_file();
}

FileStorage::~FileStorage()
{
    // Close the file
    close_file();

    // Release the buffer
    delete[] temp_buf;
    delete[] temp_buf_swap;

    // Reset the parameters
    file_count = 0;
    file_last_time = 0;
    buf_left_size = 0;
}

/**
 * @brief Implement the filename generation
 */
std::string FileStorage::generate_filename()
{
    // Get the current time
    auto current_time_in_string = get_time();

    // Generate the filename
    auto filename = BIN_FILENAME + "_" + current_time_in_string + BIN_SUFFIX;

    // Return the filename
    return filename;
}

/**
 * @brief Implement the time string generation
 */
std::string FileStorage::get_time()
{
    // Get the current time
    auto now = std::chrono::system_clock::now();

    // Get the current time in milliseconds
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    // Convert the time to long
    auto value = now_ms.time_since_epoch();

    // Convert the long value to time_t
    std::time_t time = std::chrono::duration_cast<std::chrono::seconds>(value).count();

    // Create a tm struct to store the time components
    std::tm tm_time;

    // Convert the time_t value to a tm struct
    localtime_s(&tm_time, &time); // Windows-specific function, use localtime() on other platforms

    // Create a character array to store the formatted time string
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H%M%S", &tm_time);

    // Convert the character array to a string
    std::string formatted_time(buffer);

    // Return the formatted time string
    return formatted_time;
}

/**
 * @brief Implement the time long generation
 */
long FileStorage::get_time_long()
{
    // Get the current time
    auto now = std::chrono::system_clock::now();

    // Get the current time in milliseconds
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    // Convert the time to long
    auto value = now_ms.time_since_epoch();

    // Return the time in long
    return value.count();
}

/**
* @brief Implement the file open function
*/
void FileStorage::open_file()
{
    // Generate the filename
    if (file_count == 0) {
        filename = generate_filename();
    } else {
        // flush the buffer to the file
        flush();

        // then close the file
        close_file();

        // then generate the new filename
        filename = generate_filename();
    }

    // Open the file
    file.open(filename, std::ios::out | std::ios::binary);
}

/**
 * @brief Implement the file close function
 */
void FileStorage::close_file()
{
    // Close the file
    file.close();

    // Increase the file count
    file_count++;
}

/**
 * @brief Implement the file write function
 * @param data
 * @param size
 */
void FileStorage::write_data(unsigned char* data, size_t size)
{
    // safe margin is 10% of the total buffer size
    auto safe_margin = (size_t)(BUF_SIZE * 0.1);

    // get the current time
    auto current_time = get_time_long();

    // if the buffer is not enough or the last update time is over 1 second
    if (buf_left_size < size + safe_margin || current_time - file_last_time > 1000) {
        // flush the buffer to the file
        flush();

        // if the last update time is over 1 second
        if (current_time - file_last_time > 1000) {
            // then close the file
            close_file();

            // then generate the new filename
            filename = generate_filename();

            // create a new file
            open_file();
        }
    }

    // get the current buffer position
    auto buf_pos = BUF_SIZE - buf_left_size;

    // copy the data to the buffer
    memcpy(temp_buf + buf_pos, data, size);

    // update the buffer left size
    buf_left_size -= size;
}

/**
 * @brief Implement the file flush function
 */
void FileStorage::flush()
{
    // swap the buffer
    std::swap(temp_buf, temp_buf_swap);

    // update to write size
    write_size = BUF_SIZE - buf_left_size;

    // use a thread to write the buffer to the file
    std::thread t(&FileStorage::flush_buffer, this);

    // detach the thread
    t.detach();

    // update the buffer left size
    buf_left_size = BUF_SIZE;
}

/**
 * @brief Implement the file write function
 * @param data
 * @param size
 */
void FileStorage::flush_buffer()
{
    // use the mutex lock to protect the buffer and file
    std::lock_guard<std::mutex> lock(mutex);

    // write the buffer to the file
    file.write((char*)temp_buf_swap, write_size);

    // print the debug information
    std::cout << "Write " << write_size << " bytes to " << filename << std::endl;
}