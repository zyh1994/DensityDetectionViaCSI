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
}


std::string CSIStandardData::toCSVString(const std::string& separator) const {

    // use stringstream to construct the string
    std::stringstream ss;

    // write the data to the stringstream
    ss << std::to_string(timestamp) << separator;
    ss << std::to_string(csi_length) << separator;
    ss << std::to_string(channel_number) << separator;
    ss << std::to_string(buffer_length) << separator;
    ss << std::to_string(payload_length) << separator;
    ss << std::to_string(physical_error) << separator;
    ss << std::to_string(noise_level) << separator;
    ss << std::to_string(transmission_rate) << separator;
    ss << std::to_string(channel_bandwidth) << separator;
    ss << std::to_string(number_of_tones) << separator;
    ss << std::to_string(receiver_antennas) << separator;
    ss << std::to_string(transmitter_antennas) << separator;
    ss << std::to_string(received_signal_strength) << separator;
    ss << std::to_string(rssi_antenna_0) << separator;
    ss << std::to_string(rssi_antenna_1) << separator;
    ss << std::to_string(rssi_antenna_2) << separator;

    // convert the csi matrix to json string
    ss << "[";
    for (int k = 0; k < number_of_tones; k++) {
        ss << "[";
        for (int nc_idx = 0; nc_idx < transmitter_antennas; nc_idx++) {
            ss << "[";
            for (int nr_idx = 0; nr_idx < receiver_antennas; nr_idx++) {
                ss << "["
                << std::to_string(csi_matrix[nr_idx][nc_idx][k].real)
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
    ss << "csi_matrix" << separator;
}


void CSIStandardData::update_withOpenWRT_v1(const unsigned char *buffer, int buffer_size) {

    // get the meta info
    const OpenWrt_CSI_MetaInfo_V1* meta_info = get_csi_metadata(buffer, buffer_size);

    // update the meta info
    timestamp = meta_info->tstamp;
    csi_length = meta_info->csi_len;
    channel_number = meta_info->channel;
    buffer_length =  meta_info->buf_len;
    payload_length = meta_info->payload_len;
    physical_error = meta_info->phyerr;
    noise_level = meta_info->noise;
    transmission_rate = meta_info->rate;
    channel_bandwidth = meta_info->chanBW;
    number_of_tones = meta_info->num_tones;
    receiver_antennas = meta_info->nr;
    transmitter_antennas = meta_info->nc;
    received_signal_strength = meta_info->rssi;
    rssi_antenna_0 = meta_info->rssi_0;
    rssi_antenna_1 = meta_info->rssi_1;
    rssi_antenna_2 = meta_info->rssi_2;

    // get the csi matrix
    const CSIComplex *openwrt_ptr = fill_csi_matrix(
            buffer,
            receiver_antennas,
            transmitter_antennas,
            number_of_tones);

    // update the csi matrix
    for (int nr_idx = 0; nr_idx < receiver_antennas; nr_idx++) {
        for (int nc_idx = 0; nc_idx < transmitter_antennas; nc_idx++) {
            for (int k = 0; k < number_of_tones; k++) {
                csi_matrix[nr_idx][nc_idx][k].real = index(openwrt_ptr, nc_idx, nr_idx, k, transmitter_antennas, number_of_tones)->real;
                csi_matrix[nr_idx][nc_idx][k].imag = index(openwrt_ptr, nc_idx, nr_idx, k, transmitter_antennas, number_of_tones)->imag;
            }
        }
    }
}
