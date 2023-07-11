//
// Created by Orlando Chen on 2023/07/11.
//

#ifndef PC_SAMPLING_CVMATRIX_H
#define PC_SAMPLING_CVMATRIX_H

#include <opencv2/opencv.hpp>

namespace sge {

    class CVMatrix {

    public:

        /**
         * Convert the given mat to bytes.
         * @param mat
         * @return
         */
        static char* mat2bytes(cv::Mat &mat);


        /**
         * Convert the given bytes to mat.
         * @param bytes
         * @param size
         * @param width
         * @param height
         * @param channel
         * @param dtype
         * @return
         */
        static cv::Mat bytes2mat(char* bytes, int size,
                                 int rows, int cols, int dtype);

    };
};

#endif //PC_SAMPLING_CVMATRIX_H
