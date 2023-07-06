//
// Created by Seagosoft on 6/11/2023.
//

#include "VideoHelper.h"

#include <bitset>

using namespace cv;
using namespace std;

namespace sge {

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

    cv::Writer VideoHelper::openVideoWriter(const std::string &path, VideoTypeFourCC fourcc,
                                            double fps, cv::Size frameSize) {

        // Convert the fourcc to integer.
        int fourccInt = static_cast<int>(fourcc);

        // Check the fourcc.
        VideoWriter writer(path, fourccInt, fps, frameSize);

        // Check the writer.
        if (!writer.isOpened()) {
            cout << "Cannot open the video writer!" << endl;
            exit(-1);
        }

        // Return the writer.
        return writer;
    }

};

