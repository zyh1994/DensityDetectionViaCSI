//
// Created by Orlando Chen on 2023/07/11.
//

#include "CVMatrix.h"

// malloc
#include <cstdlib>

// memcpy
#include <cstring>

// namespace sge {

    char* CVMatrix::mat2bytes(cv::Mat &mat) {

        // Get the size of the mat
        int size = mat.total() * mat.elemSize();

        // Allocate memory for the bytes
        char* bytes = (char*)malloc(size);

        // Copy the data to the bytes
        memcpy(bytes, mat.data, size);

        // Return the bytes
        return bytes;
    }

    cv::Mat CVMatrix::bytes2mat(char* bytes, int size,
                                 int rows, int cols, int dtype) {
        // Create the mat
        cv::Mat mat(rows, cols, dtype, bytes);

        // Perform deep copy to detach the data from the original vector
        return mat.clone();
    }
// };