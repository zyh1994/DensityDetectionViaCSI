#pragma once
#ifndef _TRI_HASH_CODE_H_
#define _TRI_HASH_CODE_H_

#include <string>

class HashCode {
public:

    // get the string of MD5 from binary data
    static std::string md5(void* dataset, size_t len);

    // get the string of MD5 from file
    static std::string md5(const std::string& filename);

    // get the string of sha256
    static std::string sha256(void const* dataset, size_t len);

    // get the string of sha1
    static std::string sha1(void const* dataset, size_t len);
};


#endif
