#ifndef __BIN_DATA_HELPER_H__
#define __BIN_DATA_HELPER_H__

#include <tuple>
#include <string>
#include <cstdint>

/**
 * @brief Load bytes from a file
 * @param filename
 * @return
*/
extern std::tuple<size_t, uint8_t*> load_data_from_bin(std::string filename);


/**
 * @brief Get the block size of the OpenCV format
 * @param f_data
 * @return
*/
extern size_t get_opencv_block_size(std::tuple<size_t, uint8_t*> f_data);


/**
 * @brief Get the block size of the CSI format
 * @param f_data
 * @return
*/
extern size_t get_csi_block_size(std::tuple<size_t, uint8_t*> f_data);


/**
 * @brief Parse the CSI data
 * @param bytes
 * @param size
*/
extern void parse_csi_data(uint8_t* bytes, int32_t size);


/**
 * @brief Parse the OpenCV data
 * @param bytes
 * @param size
*/
extern void parse_opencv_data(uint8_t* bytes, int32_t size);

#endif // __CSVHELPER_H__