#include "HashCode.h"
#include "SHA256.h"
#include "SHA1.h"
#include "MD5.h"

#include <memory.h>
#include <stdexcept>

std::string HashCode::md5(void* dataset, size_t len)
{
    if (dataset == nullptr) {
        throw std::invalid_argument("Nullptr detected!");
    }
    
    return md5check(dataset, len);
}

std::string HashCode::md5(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary); // 打开文件 example.txt，以二进制模式读取

    // MD5 值
    std::string hash_hex_str;

    if (!file) {
        throw std::invalid_argument("File not found!");
    }

    return md5file(filename.c_str());
}

std::string HashCode::sha256(void const* dataset, size_t len)
{
    if (dataset == nullptr) {
        throw std::invalid_argument("Nullptr detected!");
    }

    char buffer[256] = { 0 };
    auto* src_c_array = (unsigned char*)dataset;
    std::string hash_hex_str;
    picosha2::hash256_hex_string(src_c_array, src_c_array + len, hash_hex_str);
    
    // copy data
    memcpy(buffer, hash_hex_str.c_str(), 64);
    
    // return to caller
    return buffer;
}

std::string HashCode::sha1(void const* dataset, size_t len)
{
    if (dataset == nullptr) {
        throw std::invalid_argument("Nullptr detected!");
    }

    char buffer[256] = { 0 };
    SHA1 checksum;
    checksum.update(std::string((char*)dataset, len));
    std::string hash = checksum.final();

    // copy data
    memcpy(buffer, hash.c_str(), hash.length());

    // return to caller
    return buffer;
}
