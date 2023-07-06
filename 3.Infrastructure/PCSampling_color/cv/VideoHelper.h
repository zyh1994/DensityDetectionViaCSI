//
// Created by Seagosoft on 6/11/2023.
//

#ifndef DENSITYDETECTION_VIDEOHASH_H
#define DENSITYDETECTION_VIDEOHASH_H

#include <opencv2/opencv.hpp>
#include <string>

namespace sge {

    enum VideoTypeFourCC {

        // H.264/AVC codec
        H264 = cv::VideoWriter::fourcc('H', '2', '6', '4'),

        // H.265/HEVC codec
        H265 = cv::VideoWriter::fourcc('H', 'E', 'V', 'C'),

        // MPEG4-based
        MPEG_4 = cv::VideoWriter::fourcc('M', 'P', '4', 'V'),

        // Motion JPEG codec
        MJPEG = cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),

        // Uncompressed YUV format
        YUV = cv::VideoWriter::fourcc('I', '4', '2', '0'),

        // Windows Media Video (WMV) 1 codec
        WMV1 = cv::VideoWriter::fourcc('W', 'M', 'V', '1'),
    };


    class VideoHelper {

    public:

        /**
         * Open the camera with the given id.
         * Normally, the id is default to 0.
         * @param cameraId
         * @return
         */
        static cv::VideoCapture openCamera(int cameraId = 0);

        /**
         * Open the video file with the given path.
         * @param path
         * @param fourcc
         * @param fps
         * @param frameSize
         */
        static cv::Writer openVideoWriter(const std::string &path, VideoTypeFourCC fourcc,
                                          double fps, cv::Size frameSize);

    };
}

#endif //DENSITYDETECTION_VIDEOHASH_H
