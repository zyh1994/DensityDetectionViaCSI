//
// Created by Seagosoft on 6/11/2023.
//

#ifndef DENSITYDETECTION_VIDEOHASH_H
#define DENSITYDETECTION_VIDEOHASH_H

#include <opencv2/opencv.hpp>
#include <string>

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
     * Open the video file with the given filename.
     * @param filename
     * @param codec
     * @param fps
     * @param frameSize
     * @return
     */
    static cv::VideoWriter openVideoWriter(const std::string &filename, int codec, double fps, cv::Size frameSize);

    /**
     * Get the hash value of the given image.
     * @param image
     * @param hashFunction
     * @return
     */
    static std::string getHashValue(const cv::Mat& image, const std::string &hashFunction = "AverageHash");
};


#endif //DENSITYDETECTION_VIDEOHASH_H
