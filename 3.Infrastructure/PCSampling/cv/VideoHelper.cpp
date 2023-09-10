//
// Created by Seagosoft on 6/11/2023.
//
// If the Operation System is Windows, use the macro to include the opencv_world480.lib file
#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "opencv_world480d.lib")
#else
#pragma comment(lib, "opencv_world480.lib")
#endif
#endif


#include "VideoHelper.h"

#include <iostream>

using namespace cv;
using namespace std;

// namespace sge {

    int get_fourcc(VideoTypeFourCC fourcc) {
        switch (fourcc)
        {
#ifdef CV_VERSION_EPOCH
            case VideoTypeFourCC::H264:
                return CV_FOURCC('H', '2', '6', '4');
            
            case VideoTypeFourCC::H265:
                return CV_FOURCC('H', 'E', 'V', 'C');

            case VideoTypeFourCC::MPEG_4:
                return CV_FOURCC('M', 'P', '4', 'V');

            case VideoTypeFourCC::MJPEG:
                return CV_FOURCC('M', 'J', 'P', 'G');

            case VideoTypeFourCC::YUV:
                return CV_FOURCC('I', '4', '2', '0');

            case VideoTypeFourCC::WMV1:
                return CV_FOURCC('W', 'M', 'V', '1');

            default:
                std::cerr << "Unknown fourcc!" << std::endl;
                break;

#else
        case VideoTypeFourCC::H264:
            return VideoWriter::fourcc('H', '2', '6', '4');

        case VideoTypeFourCC::H265:
            return VideoWriter::fourcc('H', 'E', 'V', 'C');

        case VideoTypeFourCC::MPEG_4:
            return VideoWriter::fourcc('M', 'P', '4', 'V');
        
        case VideoTypeFourCC::MJPEG:
            return VideoWriter::fourcc('M', 'J', 'P', 'G');
        
        case VideoTypeFourCC::YUV:
            return VideoWriter::fourcc('I', '4', '2', '0');

        case VideoTypeFourCC::WMV1:
            return VideoWriter::fourcc('W', 'M', 'V', '1');

        default:
            std::cerr << "Unknown fourcc!" << std::endl;
            break;
#endif
        }

        return -1;
    }

    VideoCapture VideoHelper::openCamera(int cameraId) {

        // Open the camera.
        VideoCapture cap(cameraId);

        // Check the camera.
        if (!cap.isOpened()) {
            cout << "Cannot open the camera!" << endl;
            exit(-1);
        }

        // Return the camera.
        return cap;
    }


    void VideoHelper::closeCamera(cv::VideoCapture &cap) {
        // Release the camera.
        cap.release();
    }


    cv::VideoWriter VideoHelper::openVideoWriter(const std::string &path, int fourcc,
                                            double fps, cv::Size frameSize) {
        // Check the fourcc.
        if (fourcc == -1) {
            cout << "Unknown fourcc!" << endl;
            exit(-1);
        }

        // Open the writer.
        VideoWriter writer(path, fourcc, fps, frameSize);

        // Check the writer.
        if (!writer.isOpened()) {
            cout << "Cannot open the video writer!" << endl;
            exit(-1);
        }

        // Return the writer.
        return writer;
    }


    void VideoHelper::closeVideoWriter(cv::VideoWriter &writer) {
        // Release the writer.
        writer.release();
    }


    void VideoHelper::setCameraResolution(VideoCapture &cap, int width, int height) {
#ifdef CV_VERSION_EPOCH
        /* Set the video resolution to width x height */
        cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);
#else
        /* Set the video resolution to width x height */
        cap.set(CAP_PROP_FRAME_WIDTH, width);
        cap.set(CAP_PROP_FRAME_HEIGHT, height);
#endif
    }

// };

