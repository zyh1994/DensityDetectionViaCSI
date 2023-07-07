//
// Created by Orlando Chen on 2023/07/02.
//

#include "FileStorage.h"

#include "../cv/VideoHelper.h"
#include "../csi/CSIHelper.h"

namespace sge {

    std::string convert_csi_bytes(unsigned char* data, size_t size) {
        std::stringstream ss;

        // Use the csi_struct to parse the metadata
        csi_struct* csi_meta = get_csi_metadata(data, static_cast<int>(size));
        COMPLEX* csi_matrix  = get_csi_matrix(data, static_cast<int>(size)); 


        return ss.str();
    }

    FileStorage::FileStorage(std::string basename) {
        basename = std::move(basename);
    }

    FileStorage::~FileStorage() {
        close_file();
    }

    void FileStorage::open_file() {
        if (csv_file.is_open()) {
            csv_file.close();
        }

        // 生成新的文件名
        auto filename = new_filename();

        // 创建新的文件
        csv_file.open(filename, std::ios::binary | std::ios::app);

        // Open new video writer for a new mp4 file
        writer = VideoHelper::openVideoWriter(filename + ".mp4",
                                              get_fourcc(VideoTypeFourCC::MPEG_4),
                                              30, cv::Size(640, 480));
    }

    void FileStorage::close_file() {
        if (csv_file.is_open()) {
            csv_file.close();
        }

        if (writer.isOpened()) {
            // Close the video writer
            VideoHelper::closeVideoWriter(writer);
        }
    }

    void FileStorage::write(long long timestamp, cv::Mat &mat,
                            unsigned char *data, size_t size) {

    }

    std::string FileStorage::new_filename() {
        // 获取当前时间
        std::time_t now = std::time(nullptr);

        // 将时间转换为本地时间结构
        std::tm *localTime = std::localtime(&now);

        // 创建一个字符串流对象
        std::stringstream ss;

        // 格式化时间为指定格式，并将其写入字符串流
        ss << basename << "_"
           << std::put_time(localTime, "%Y%m%d%H%M");

        // 从字符串流中获取格式化后的时间字符串
        std::string formattedTime = ss.str();

        // 返回格式化后的文件名
        return formattedTime;
    }
}
