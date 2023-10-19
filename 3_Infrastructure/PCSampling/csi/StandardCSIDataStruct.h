#ifndef __STANDARD_CSI_DATA_STRUCT_H__
#define __STANDARD_CSI_DATA_STRUCT_H__

#include <cstdlib>
#include <iostream>
#include <string>

#ifndef CSI_STD_DEF_V1
#define CSI_STD_DEF_V1
#define STD_DEFAULT_TX      3   // Transmitter
#define STD_DEFAULT_RX      3   // Receiver
#define STD_DEFAULT_TONES   116 // Number of subcarriers
#endif


struct complex {
    int32_t real;
    int32_t imag;
};


struct CSIStandardData {
    uint64_t    timestamp;
    uint16_t    csi_length;
    uint16_t    channel_number;
    uint16_t    buffer_length;
    uint16_t    payload_length;
    uint8_t     physical_error;
    uint8_t     noise_level;
    uint8_t     transmission_rate;
    uint8_t     channel_bandwidth;
    uint8_t     number_of_tones;
    uint8_t     receiver_antennas;
    uint8_t     transmitter_antennas;
    uint8_t     received_signal_strength;
    uint8_t     rssi_antenna_0;
    uint8_t     rssi_antenna_1;
    uint8_t     rssi_antenna_2;
    complex     csi_matrix[STD_DEFAULT_TX][STD_DEFAULT_RX][STD_DEFAULT_TONES] = { 0 };
};


struct CSIStandardDataWithChecksum {
    uint32_t    crc32;                                  // CRC32 checksum of the data (bytes only)
    uint8_t     bytes[sizeof(CSIStandardData)];         // sizeof(struct CSIDataStruct) is 8240
};


struct OpenCVDataFrameWithChecksum {
    uint32_t    timestamp;                              // timestamp of the sampling
    uint32_t    crc32;                                  // CRC32 checksum of the data (bytes only)
    uint16_t    width;                                  // width of the frame
    uint16_t    height;                                 // height of the frame
    uint16_t    channels;                               // channels of the frame
    uint16_t    reserved;                               // reserved for future use
    uint8_t     bytes[320 * 180];                       // 320 x 180 x Grayscale
};


#define CVDATA_CHECKSUM_SIZE         sizeof(OpenCVDataFrameWithChecksum)    // The size is 57616
#define CSISTD_CHECKSUM_SIZE         sizeof(CSIStandardDataWithChecksum)    // The size is 8248 


#endif // !__STANDARD_CSI_DATA_STRUCT_H__