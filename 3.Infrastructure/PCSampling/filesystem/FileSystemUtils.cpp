//
// Created by Orlando Chen on 2023/08/23.
//

#include "FileSystemUtils.h"

#include <sstream>
#include <iomanip>


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
    return formattedTime + ".bin";
};