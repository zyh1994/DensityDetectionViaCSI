//
// Created by Orlando Chen on 2023/08/23.
//

#ifndef PC_SAMPLING_FILESYSTEMUTILS_H
#define PC_SAMPLING_FILESYSTEMUTILS_H

#include <string>

std::string gen_filename();

const uint32_t calculate_crc32(const unsigned char *buf, size_t len);

const uint64_t timestamp();

#endif //PC_SAMPLING_FILESYSTEMUTILS_H
