#pragma once
#ifndef __LOWBITSOPERATION_HPP__
#define __LOWBITSOPERATION_HPP__

#include <cstdint>
#include <iostream>
#include <strstream>

/**
 * @brief Convert the data to a negative number if the highest bit is 1
 * @param data
 * @param max_bit
 * @return
*/
extern int convert_negative_data(uint32_t data, int max_bit);


/**
 * @brief Change the order of the bits in the buffer
 * @param buf
 * @param buf_size
*/
extern void change_bytes_order(uint8_t* buf, const int32_t buf_size);


/**
 * @brief Change the order of the bits in the buffer
 * @tparam T
 * @param buf
 * @param buf_size
*/
template <typename T>
void change_data_order(T* buf, const int32_t buf_size) {
    for (int i = 0; i < buf_size / 2; i++) {
        std::swap(buf[i], buf[buf_size - i - 1]);
    }
}


/**
 * @brief Write 16 bits to the buffer by little endian
 * @param buf
 * @param idx
 * @param data
 */
extern void write_16bits_by_little_endian(unsigned char* buf, int& idx, uint32_t data);


/**
 * @brief Write 16 bits to the buffer by big endian
 * @param buf
 * @param idx
 * @param data
 */
extern void write_16bits_by_big_endian(unsigned char* buf, int& idx, uint32_t data);


/**
 * @brief Write 32 bits to the buffer by little endian
 * @param buf
 * @param idx
 * @param data
 */
extern void write_32bits_by_little_endian(unsigned char* buf, int& idx, uint32_t data);

/**
 * @brief Write 32 bits to the buffer by big endian
 * @param buf
 * @param idx
 * @param data
 */
extern void write_32bits_by_big_endian(unsigned char* buf, int& idx, uint32_t data);


/**
 * @brief Write 64 bits to the buffer by little endian
 * @param buf
 * @param idx
 * @param data
*/
extern void write_64bits_by_big_endian(unsigned char* buf, int& idx, uint64_t data);


/**
 * @brief Write 64 bits to the buffer by little endian
 * @param buf
 * @param idx
 * @param data
*/
extern void write_64bits_by_little_endian(unsigned char* buf, int& idx, uint64_t data);

/**
 * @brief Write 64 bits to the buffer by big endian
 * @param buf
 * @param idx
 * @param data
*/
extern uint64_t read_64bits_by_big_endian(const unsigned char* buf, int& idx);

/**
 * @brief Read 64 bits from the buffer by little endian
 * @param buf
 * @param idx
 * @return
*/
extern uint64_t read_64bits_by_little_endian(const unsigned char* buf, int& idx);


/**
 * @brief Read 16 bits from the buffer by little endian
 * @param buf
 * @param idx
 * @return
 */
extern uint32_t read_16bits_by_little_endian(const unsigned char* buf, int& idx);


/**
 * @brief Read 16 bits from the buffer by big endian
 * @param buf
 * @param idx
 * @return
 */
extern uint32_t read_16bits_by_big_endian(const unsigned char* buf, int& idx);


/**
 * @brief Read 32 bits from the buffer by little endian
 * @param buf
 * @param idx
 * @return
 */
extern uint32_t read_32bits_by_little_endian(const unsigned char* buf, int& idx);


/**
 * @brief Read 32 bits from the buffer by big endian
 * @param buf
 * @param idx
 * @return
 */
extern uint32_t read_32bits_by_big_endian(const unsigned char* buf, int& idx);


/**
 * @brief Merge two integers into one integer
 * @param int1
 * @param bits_left1
 * @param int2
 * @param bits_left2
 * @return
 */
extern uint32_t merge_big_endian(uint32_t int1, uint32_t bits_left1, uint32_t int2, uint32_t bits_left2);


/**
 * @brief Merge two integers into one integer
 * @param int1
 * @param bits_left1
 * @param int2
 * @param bits_left2
 * @return
*/
extern uint32_t merge_little_endian(uint32_t int1, uint32_t bits_left1, uint32_t int2, uint32_t bits_left2);


/**
 * @brief Split the data into two integers
 * @param data
 * @param bits
 * @param extracted
 * @param remainder
*/
extern void split_bits_big_endian(uint32_t data, int16_t bits, uint32_t& extracted, uint32_t& remainder);


/**
 * @brief Split the data into two integers
 * @param data
 * @param bits
 * @param extracted
 * @param remainder
*/
extern void split_bits_little_endian(uint32_t data, int16_t bits, uint32_t& extracted, uint32_t& remainder);


/**
 * @brief Change the order of the bits in the buffer
 * @param data
 * @param bits
 * @return
*/
extern uint32_t bits_inverse(uint32_t data, int bits);


/**
 * @brief Check if the system is big endian
 * @return
*/
extern bool is_big_endian();

#endif // !__LOWBITSOPERATION_HPP__
