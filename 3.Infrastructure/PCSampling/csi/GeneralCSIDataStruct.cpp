#include "GeneralCSIDataStruct.h"
#include <sstream>
#include <string>


CSIStandardData::CSIStandardData() {
    timestamp = 0;
    csi_length = 0;
    channel_number = 0;
    buffer_length = 0;
    payload_length = 0;
    physical_error = 0;
    noise_level = 0;
    transmission_rate = 0;
    channel_bandwidth = 0;
    number_of_tones = 0;
    receiver_antennas = 0;
    transmitter_antennas = 0;
    received_signal_strength = 0;
    rssi_antenna_0 = 0;
    rssi_antenna_1 = 0;
    rssi_antenna_2 = 0;

    // initialize the csi matrix
    for (auto & i : csi_matrix) {
        for (auto & j : i) {
            for (auto & k : j) {
                k.real = 0;
                k.imag = 0;
            }
        }
    }
}


std::string CSIStandardData::toCSVString(const std::string& separator) const {

    // use stringstream to construct the string
    std::stringstream ss;

    // write the data to the stringstream
    ss << std::to_string((uint64_t)timestamp) << separator;
    ss << std::to_string((int32_t)csi_length) << separator;
    ss << std::to_string((int32_t)channel_number) << separator;
    ss << std::to_string((int32_t)buffer_length) << separator;
    ss << std::to_string((int32_t)payload_length) << separator;
    ss << std::to_string((int32_t)physical_error) << separator;
    ss << std::to_string((int32_t)noise_level) << separator;
    ss << std::to_string((int32_t)transmission_rate) << separator;
    ss << std::to_string((int32_t)channel_bandwidth) << separator;
    ss << std::to_string((int32_t)number_of_tones) << separator;
    ss << std::to_string((int32_t)receiver_antennas) << separator;
    ss << std::to_string((int32_t)transmitter_antennas) << separator;
    ss << std::to_string((int32_t)received_signal_strength) << separator;
    ss << std::to_string((int32_t)rssi_antenna_0) << separator;
    ss << std::to_string((int32_t)rssi_antenna_1) << separator;
    ss << std::to_string((int32_t)rssi_antenna_2) << separator;

    // convert the csi matrix to json string
    ss << "[";
    for (int k = 0; k < number_of_tones; k++) {
        ss << "[";
        for (int nc_idx = 0; nc_idx < transmitter_antennas; nc_idx++) {
            ss << "[";
            for (int nr_idx = 0; nr_idx < receiver_antennas; nr_idx++) {
                ss << "["
                << std::to_string((int32_t)csi_matrix[nr_idx][nc_idx][k].real)
                << ", "
                << std::to_string(csi_matrix[nr_idx][nc_idx][k].imag)
                << "]";
            }
            ss << "]";
        }
        ss << "]";
    }
    ss << "]";

    // return the string
    return ss.str();
}


std::string CSIStandardData::toCSVHeader(const std::string& separator) {

    // use stringstream to construct the string
    std::stringstream ss;

    // write the data to the stringstream
    ss << "timestamp" << separator;
    ss << "csi_length" << separator;
    ss << "channel_number" << separator;
    ss << "buffer_length" << separator;
    ss << "payload_length" << separator;
    ss << "physical_error" << separator;
    ss << "noise_level" << separator;
    ss << "transmission_rate" << separator;
    ss << "channel_bandwidth" << separator;
    ss << "number_of_tones" << separator;
    ss << "receiver_antennas" << separator;
    ss << "transmitter_antennas" << separator;
    ss << "received_signal_strength" << separator;
    ss << "rssi_antenna_0" << separator;
    ss << "rssi_antenna_1" << separator;
    ss << "rssi_antenna_2" << separator;
    ss << "csi_matrix";

    // return the string
    return ss.str();
}

#include "../csi/OpenWRT_v1.h"

void CSIStandardData::updateWithOpenWRTv1(uint8_t *buffer, size_t buffer_size) {

    // Get the CSI meta information from buffer
    auto meta_info = get_csi_metadata(buffer, buffer_size);

    // Update the meta information
    timestamp = meta_info->timestamp;
    csi_length = meta_info->csi_len;
    channel_number = meta_info->channel;
    buffer_length = meta_info->buf_len;
    payload_length = meta_info->payload_len;
    physical_error = meta_info->physical_err;
    noise_level = meta_info->noise;
    transmission_rate = meta_info->rate;
    channel_bandwidth = meta_info->bandwidth;
    number_of_tones = meta_info->num_tones;
    receiver_antennas = meta_info->nr;
    transmitter_antennas = meta_info->nc;
    received_signal_strength = meta_info->rssi;
    rssi_antenna_0 = meta_info->rssi_0;
    rssi_antenna_1 = meta_info->rssi_1;
    rssi_antenna_2 = meta_info->rssi_2;

    //////// Update the CSI matrix ////////

    // Convert the CSI matrix to an int32_t array with one dimension
    auto* csi_matrix_ptr = reinterpret_cast<int32_t*>(csi_matrix);

    // Use the function decode_openwrt_v1 to get the CSI matrix
    decode_openwrt_v1(buffer + CSI_META_LEN + 2, meta_info->csi_len, csi_matrix_ptr);
}
