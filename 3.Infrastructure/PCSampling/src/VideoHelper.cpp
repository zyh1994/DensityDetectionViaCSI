//
// Created by Seagosoft on 6/11/2023.
//

#include "../inc/VideoHelper.h"

#include <bitset>

using namespace cv;
using namespace std;

/**
 * @brief Calculate the average hash value of the given image.
 * @param src
 * @return
 */
cv::Mat averageHash(const cv::Mat& src){
    cv::Mat img, gray;
    cv::resize(src, img, cv::Size(8, 8), 0, 0, cv::INTER_AREA);
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    uchar avg = cv::mean(gray)[0];
    cv::Mat hash = gray >= avg;
    return hash;
}

/**
 * @brief Calculate the difference hash value of the given image.
 * @param src
 * @return
*/
cv::Mat differenceHash(const cv::Mat& src){
    cv::Mat img, gray, diff;
    cv::resize(src, img, cv::Size(9, 8), 0, 0, cv::INTER_AREA);
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::absdiff(gray(cv::Rect(1, 0, 8, 8)), gray(cv::Rect(0, 0, 8, 8)), diff);
    cv::Mat hash = diff >= 0;
    return hash;
}

/**
 * @brief Calculate the perceptual hash value of the given image.
 * @param src
 * @return
*/
cv::Mat perceptualHash(const cv::Mat& src){
    cv::Mat img, gray, dct, hash;
    cv::resize(src, img, cv::Size(32, 32), 0, 0, cv::INTER_AREA);
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::dct(cv::Mat_<float>(gray), dct);
    dct = dct(cv::Rect(0, 0, 8, 8));
    uchar avg = cv::mean(dct)[0];
    hash = dct >= avg;
    return hash;
}

/**
 * @brief Calculate the Marr-Hildreth hash value of the given image.
 * @param src
 * @return
*/
std::string hashToString(const cv::Mat& hash){
    std::string hashString;
    for (int i = 0; i < hash.rows; ++i){
        for (int j = 0; j < hash.cols; ++j){
            hashString += std::to_string(hash.at<uchar>(i,j));
        }
    }
    return hashString;
}

VideoCapture VideoHelper::openCamera(int cameraId) {
    VideoCapture cap(cameraId);
    if (!cap.isOpened()) {
        cout << "Cannot open the camera!" << endl;
        exit(-1);
    }
    return cap;
}

VideoWriter VideoHelper::openVideoWriter(const string &filename, int codec, double fps, Size frameSize) {

    VideoWriter writer(filename, codec, fps, frameSize);
    if (!writer.isOpened()) {
        cout << "Cannot open the video writer!" << endl;
        exit(-1);
    }
    return writer;
}

std::string VideoHelper::getHashValue(const Mat &image, const string &hashFunction) {

    if (hashFunction == "AverageHash"){
        return hashToString(averageHash(image));
    } else if (hashFunction == "DifferenceHash"){
        return hashToString(differenceHash(image));
    } else if (hashFunction == "PerceptualHash"){
        return hashToString(perceptualHash(image));
    } else {
        cout << "The hash function is not supported!" << endl;
        exit(-1);
    }
}
