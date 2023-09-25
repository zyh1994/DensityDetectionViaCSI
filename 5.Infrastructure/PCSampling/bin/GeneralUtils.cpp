//
// Created by Orlando Chen on 2023/08/23.
//

// If the Operation System is Windows, use _CRT_SECURE_NO_WARNINGS to disable the security check
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif


#include "GeneralUtils.h"
#include "crc32Table.h"

#include <iomanip>
#include <sstream>


std::string gen_filename() {
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
};


const uint32_t calculate_crc32(const unsigned char *buf, size_t len) {

    const uint8_t* u8_data = reinterpret_cast<const uint8_t*>(buf);
    uint32_t crc = ~0U;

    while (len--) {
        crc = crc32_table[(crc ^ *u8_data++) & 0xFF] ^ (crc >> 8);
    }

    return ~crc;
}

#include <chrono>

const uint64_t timestamp() {
    using namespace std::chrono;
    uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    return ms;
}