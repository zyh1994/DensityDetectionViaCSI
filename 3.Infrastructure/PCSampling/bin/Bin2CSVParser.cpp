#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <cstring>

#include <opencv2/opencv.hpp>

#include "Bin2CSVParser.h"
#include "SynchronousBinProcessor.h"
#include "../csi/StandardCSIDataProcessing.h"


// Creat an 1 GB buffer
uint8_t *buffer = new uint8_t[1024 * 1024 * 1024];
CSIStandardDataProcessingClass *csi_std_class = new CSIStandardDataProcessingClass();
CSIStandardDataWithChecksum *csi_checksum_data = new CSIStandardDataWithChecksum();
OpenCVDataFrameWithChecksum *opencv_checksum_data = new OpenCVDataFrameWithChecksum();


std::tuple<size_t, uint8_t*> load_data_from_bin(std::string filename) {
    // If the input file is not a .bin file, then exit
    if (filename.find(".bin") == std::string::npos)
    {
        std::cout << "Input file must be a .bin file" << std::endl;
        exit(1);
    }

    // If the input file does not exist, then exit
    if (!std::filesystem::exists(filename))
    {
        std::cout << "Input file does not exist" << std::endl;
        exit(1);
    }

    // Get the size of the file
    auto file_size = std::filesystem::file_size(filename);

    // Open the file and read all the bytes
    std::fstream file(filename, std::ios::in | std::ios::binary);
    file.read((char*)buffer, file_size);    

    // Return the file size and the bytes
    return std::make_tuple(file_size, buffer);
}


size_t get_opencv_block_size(std::tuple<size_t, uint8_t*> f_data) {

    // Get the bytes from the tuple
    auto ptr_bytes = std::get<1>(f_data);

    // Get the size of the cv data from the bytes
    size_t data_size = 0;
    memcpy(&data_size, ptr_bytes, sizeof(size_t));

    // Return the size
    return data_size;
}


size_t get_csi_block_size(std::tuple<size_t, uint8_t*> f_data) {

    // Get the bytes from the tuple
    auto ptr_bytes = std::get<1>(f_data);

    // Get the size of the csi data from the bytes
    size_t data_size = 0;
    memcpy(&data_size, ptr_bytes + sizeof(size_t), sizeof(size_t));

    // Return the size
    return data_size;
}


void parse_csi_data(uint8_t* bytes, int32_t size) {

    // The index
    int32_t idx = 0;

    // External function for saving the data
    extern void save_csi_data(std::ofstream &ofs, CSIStandardDataWithChecksum * frame);
    extern void save_csi_header(std::ofstream &ofs, CSIStandardDataWithChecksum * frame);

    // Create an output file stream
    std::ofstream ofs;

    // Create a CSV file for saving the data
    ofs.open("csi.csv", std::ios::out | std::ios::trunc);

    // Save the header
    save_csi_header(ofs, csi_checksum_data);

    // Use a while loop to parse the data
    while (size > 0) {

        // Copy the data to the CSIWifiDataFrame
        memcpy(csi_checksum_data, bytes + idx, sizeof(CSIStandardDataWithChecksum));

        // Save the frame as a CSV
        save_csi_data(ofs, csi_checksum_data);

        // Increase the index by sizeof(CSIWifiDataFrame)
        idx += sizeof(CSIStandardDataWithChecksum);

        // Decrease the size by sizeof(CSIWifiDataFrame)
        size -= sizeof(CSIStandardDataWithChecksum);
    }

    // Close the file
    ofs.close();
};


void save_csi_header(std::ofstream &ofs, CSIStandardDataWithChecksum* frame) {

    // From CSIStandardDataWithChecksum get CSIStandardData
    auto* ptr_std_data = reinterpret_cast<CSIStandardData*>(frame->bytes);

    // Update the data
    csi_std_class->updateWithCSIDataStruct(ptr_std_data);

    // Write the header of columns to the CSV file
    ofs << csi_std_class->toCSVHeader() << std::endl;
}


// void save_csi_data(std::ofstream &ofs, CSIStandardDataWithChecksum* frame) {
//     // From CSIStandardDataWithChecksum get CSIStandardData
//     auto* ptr_std_data = reinterpret_cast<CSIStandardData*>(frame->bytes);

//     // Update the data
//     csi_std_class->updateWithCSIDataStruct(ptr_std_data);

//     // Write the data to the CSV file
//     ofs << csi_std_class->toCSVString() << std::endl;
// };


void parse_opencv_data(uint8_t* bytes, int32_t size) {

    // The index
    int32_t idx = 0;

    // External function for saving the data
    extern void save_thumbnail(OpenCVDataFrameWithChecksum* frame);

    // Use a while loop to parse the data
    while (size > 0) {

        // Copy the data to the OpenCVFrame
        memcpy(opencv_checksum_data, bytes + idx, sizeof(OpenCVDataFrameWithChecksum));

        // Save the frame as a thumbnail picture
        save_thumbnail(opencv_checksum_data);

        // Increase the index by sizeof(OpenCVFrame)
        idx += sizeof(OpenCVDataFrameWithChecksum);

        // Decrease the size by sizeof(OpenCVFrame)
        size -= sizeof(OpenCVDataFrameWithChecksum);
    }
};


void save_thumbnail(OpenCVDataFrameWithChecksum* frame) {

    // Use the timestamp as the name of the picture
    auto timestamp_str = std::to_string(frame->timestamp);

    // Create a Mat object from the bytes
    cv::Mat mat(frame->height, frame->width, CV_8UC1, frame->bytes);

    // Decode the Mat object
    //cv::Mat decoded_mat = cv::imdecode(mat, cv::IMREAD_GRAYSCALE);

    // Save the decoded Mat object as a picture
    cv::imwrite(timestamp_str + ".jpg", mat);

    // Release the memory of the Mat object
    mat.release();
    //decoded_mat.release();
}