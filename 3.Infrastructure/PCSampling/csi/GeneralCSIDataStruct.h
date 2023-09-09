

#ifndef __GENERAL_CSI_DATA_STRUCT_H__
#define __GENERAL_CSI_DATA_STRUCT_H__

#include <cstdlib>
#include <iostream>
#include <string>

#include "OpenWRT_v1.h"


struct complex {
    int32_t real;
    int32_t imag;
};


class CSIStandardData {

private:

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
    complex     csi_matrix[DEFAULT_TX][DEFAULT_RX][DEFAULT_TONES] = {0};

public:
    inline uint64_t getTimestamp() const { return timestamp; }
    inline int32_t getCSILength() const { return csi_length; }
    inline int32_t getChannelNumber() const { return channel_number; }
    inline int32_t getBufferLength() const { return buffer_length; }
    inline int32_t getPayloadLength() const { return payload_length; }
    inline int32_t getPhysicalError() const { return physical_error; }
    inline int32_t getNoiseLevel() const { return noise_level; }
    inline int32_t getTransmissionRate() const { return transmission_rate; }
    inline int32_t getChannelBandwidth() const { return channel_bandwidth; }
    inline int32_t getNumberOfTones() const { return number_of_tones; }
    inline int32_t getReceiverAntennas() const { return receiver_antennas; }
    inline int32_t getTransmitterAntennas() const { return transmitter_antennas; }
    inline int32_t getReceivedSignalStrength() const { return received_signal_strength; }
    inline int32_t getRSSIAntenna0() const { return rssi_antenna_0; }
    inline int32_t getRSSIAntenna1() const { return rssi_antenna_1; }
    inline int32_t getRSSIAntenna2() const { return rssi_antenna_2; }

public:

    CSIStandardData();

    /**
     * @brief Convert current record to a CSV string
     * @param separator
     * @return
     */
    std::string toCSVString(const std::string& separator="\t") const;

    /**
     * @brief Get the CSV header
     * @param separator
     * @return
     */
    static std::string toCSVHeader(const std::string& separator="\t");

    /**
     * @brief Update the CSI data with the OpenWRT_v1 format
     * @param buffer
     * @param buffer_size
     */
    void updateWithOpenWRTv1(uint8_t *buffer, size_t buffer_size);

    /**
     * @brief Access the CSI matrix data directly
     * @return
     */
    inline const int32_t* matrix() {
        // Convert the CSI matrix to an int32_t array with one dimension
        auto* csi_matrix_ptr = reinterpret_cast<int32_t*>(csi_matrix);

        // Return the pointer
        return csi_matrix_ptr;
    };

    /**
     * @brief Access the CSI matrix data by given index
     * @param ntr
     * @param nrx
     * @param subcarrier
     * @return
     */
    inline const complex* get(int ntr, int nrx, int subcarrier) {
        return &csi_matrix[ntr][nrx][subcarrier];
    };


    /**
     * @brief Set the CSI matrix data by given index
     * @param ntr
     * @param nrx
     * @param subcarrier
     * @param value
     */
    inline void set(int ntr, int nrx, int subcarrier, complex* value) {
        csi_matrix[ntr][nrx][subcarrier] = *value;
    };
};

#endif