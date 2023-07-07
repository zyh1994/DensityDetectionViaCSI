//
// Created by Orlando Chen on 2023/07/02.
//

#include "FileStorage.h"

#include "../cv/VideoHelper.h"
#include "../csi/CSIHelper.h"

namespace sge {

    std::string convert_csi_matrix(COMPLEX* matrix, int nc, int nr, int tones) {
        std::stringstream ss;

        // Convert the one-dimensional matrix to a three-dimensional matrix
        for (int k = 0; k < tones; k++) {

            ss << "[";
            for (int nc_idx = 0; nc_idx < nc; nc_idx++) {
                ss << "[";
                for (int nr_idx = 0; nr_idx < nr; nr_idx++) {
                    // Calculate the index of the current element
                    int idx = k * nc * nr + nc_idx * nr + nr_idx;
                    ss << "["
                    << matrix[idx].real
                    << ", "
                    << matrix[idx].imag;

                    // if the last element, remove the comma
                    if (nr_idx == nr - 1) {
                        ss << "]";
                    } else {
                        ss << "],";
                    }
                }

                // if the last element, remove the comma
                if (nc_idx == nc - 1) {
                    ss << "]";
                } else {
                    ss << "],";
                }
            }

            // if the last element, remove the comma
            if (k == tones - 1) {
                ss << "]";
            } else {
                ss << "],";
            }
        }

        return ss.str();
    }

    std::string convert_csi_bytes(csi_struct* csi_meta,
                              COMPLEX* csi_matrix,
                              long long timestamp,
                              std::string delimiter="\t") {
        std::stringstream ss;

        // Write the row data
        ss << std::to_string(timestamp) << delimiter;
        ss << std::to_string(csi_meta->csi_len) << delimiter;
        ss << std::to_string(csi_meta->channel) << delimiter;
        ss << std::to_string(csi_meta->buf_len) << delimiter;
        ss << std::to_string(csi_meta->payload_len) << delimiter;
        ss << std::to_string(csi_meta->phyerr) << delimiter;
        ss << std::to_string(csi_meta->noise) << delimiter;
        ss << std::to_string(csi_meta->rate) << delimiter;
        ss << std::to_string(csi_meta->chanBW) << delimiter;
        ss << std::to_string(csi_meta->num_tones) << delimiter;
        ss << std::to_string(csi_meta->nr) << delimiter;
        ss << std::to_string(csi_meta->nc) << delimiter;
        ss << std::to_string(csi_meta->rssi) << delimiter;
        ss << std::to_string(csi_meta->rssi_0) << delimiter;
        ss << std::to_string(csi_meta->rssi_1) << delimiter;
        ss << std::to_string(csi_meta->rssi_2) << delimiter;
        ss << convert_csi_matrix(csi_matrix,
                                csi_meta->nc,
                                csi_meta->nr,
                                csi_meta->num_tones) << "\n";

        // Return the string
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
