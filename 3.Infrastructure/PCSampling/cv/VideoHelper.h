//
// Created by Seagosoft on 6/11/2023.
//

#ifndef DENSITYDETECTION_VIDEOHASH_H
#define DENSITYDETECTION_VIDEOHASH_H

#include <string>

#include "CVMatrix.h"

using namespace cv;

namespace sge {

    enum VideoTypeFourCC {
        H264,
        H265,
        MPEG_4,
        MJPEG,
        YUV,
        WMV1,
    };

    extern int get_fourcc(VideoTypeFourCC fourcc);


    // Public inherit the sge::CVMatrix class
    class VideoHelper : public CVMatrix {

    public:

        /**
         * Open the camera with the given id.
         * Normally, the id is default to 0.
         * @param cameraId
         * @return
         */
        static cv::VideoCapture openCamera(int cameraId = 0);

        /**
         * Close the camera.
         * @param cap
         */
        static void closeCamera(cv::VideoCapture &cap);

        /**
         * Set the camera resolution.
         * @param cap
         * @param width
         * @param height
         */
        static void setCameraResolution(cv::VideoCapture &cap, int width, int height);

        /**
         * Open the video file with the given path.
         * @param path
         * @param fourcc
         * @param fps
         * @param frameSize
         */
        static cv::VideoWriter openVideoWriter(const std::string &path, int fourcc, 
                double fps, cv::Size frameSize);


        /**
         * Close the video writer.
         * @param writer
         */
        static void closeVideoWriter(cv::VideoWriter &writer);
    };
}

#endif //DENSITYDETECTION_VIDEOHASH_H
