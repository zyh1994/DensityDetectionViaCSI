

#ifndef __GENERAL_CSI_DATA_STRUCT_H__
#define __GENERAL_CSI_DATA_STRUCT_H__

#include <cstdlib>
#include <iostream>
#include <string>

#include "OpenWRT_v1.h"


class CSIStandardData {

private:

    u_int64_t   timestamp{};
    u_int16_t   csi_length{};
    u_int16_t   channel_number{};
    u_int16_t   buffer_length{};
    u_int16_t   payload_length{};
    u_int8_t    physical_error{};
    u_int8_t    noise_level{};
    u_int8_t    transmission_rate{};
    u_int8_t    channel_bandwidth{};
    u_int8_t    number_of_tones{};
    u_int8_t    receiver_antennas{};
    u_int8_t    transmitter_antennas{};
    u_int8_t    received_signal_strength{};
    u_int8_t    rssi_antenna_0{};
    u_int8_t    rssi_antenna_1{};
    u_int8_t    rssi_antenna_2{};
    CSIComplex  csi_matrix[3][3][128]{};


public:

    CSIStandardData();


    std::string toCSVString(const std::string& separator="\t") const;


    std::string toCSVHeader(const std::string& separator="\t");


    void update_withOpenWRT_v1(const unsigned char *buffer, int buffer_size);
}; 

#endif