#include "bin/SynchronousBinProcessor.h"
#include "bin/Bin2CSVParser.h"
#include "csi/OpenWRT_v1.h"
#include "csi/StandardCSIDataProcessing.h"


int main(int argc, char* argv[])
{
    //if (argc != 2)
    //{
    //    std::cout << "Usage: bin2csv <input file>" << std::endl;
    //    return 1;
    //}

    // Get the data of the file
    auto f_data = load_data_from_bin("./202309101257.bin");

    // Print out the size of the file
    std::cout << "Input file size: " << std::get<0>(f_data) << " Bytes" << std::endl;
    std::cout << "OpenCV block size: " << get_opencv_block_size(f_data) << " Bytes" << std::endl;
    std::cout << "CSI block size: " << get_csi_block_size(f_data) << " Bytes" << std::endl;

    // Parse the OpenCV frames from bytes
    uint8_t* ptr_opencv = std::get<1>(f_data) + 2 * sizeof(size_t);
    parse_opencv_data(ptr_opencv, get_opencv_block_size(f_data));

    std::cout << "Done parsing OpenCV frames" << std::endl;

    // Parse the CSI frames from bytes
    uint8_t* ptr_csi = std::get<1>(f_data) + 2 * sizeof(size_t) + get_opencv_block_size(f_data);
    parse_csi_data(ptr_csi, get_csi_block_size(f_data));

    std::cout << "Done parsing CSI frames" << std::endl;

    return 0;
}