#ifndef __STANDARD_CSI_DATA_PROCESSING_H__
#define __STANDARD_CSI_DATA_PROCESSING_H__

#include "StandardCSIDataStruct.h"



class CSIStandardDataProcessingClass {

private:
    CSIStandardData data;

public:
    inline uint64_t getTimestamp() const { return data.timestamp; }
    inline int32_t getCSILength() const { return data.csi_length; }
    inline int32_t getChannelNumber() const { return data.channel_number; }
    inline int32_t getBufferLength() const { return data.buffer_length; }
    inline int32_t getPayloadLength() const { return data.payload_length; }
    inline int32_t getPhysicalError() const { return data.physical_error; }
    inline int32_t getNoiseLevel() const { return data.noise_level; }
    inline int32_t getTransmissionRate() const { return data.transmission_rate; }
    inline int32_t getChannelBandwidth() const { return data.channel_bandwidth; }
    inline int32_t getNumberOfTones() const { return data.number_of_tones; }
    inline int32_t getReceiverAntennas() const { return data.receiver_antennas; }
    inline int32_t getTransmitterAntennas() const { return data.transmitter_antennas; }
    inline int32_t getReceivedSignalStrength() const { return data.received_signal_strength; }
    inline int32_t getRSSIAntenna0() const { return data.rssi_antenna_0; }
    inline int32_t getRSSIAntenna1() const { return data.rssi_antenna_1; }
    inline int32_t getRSSIAntenna2() const { return data.rssi_antenna_2; }

public:

    CSIStandardDataProcessingClass();

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
     * @brief Update the CSI data with the raw data of CSIDataStruct
     * @param buffer
     */
    void updateWithCSIDataStruct(CSIStandardData* csi_data);

    /**
     * @brief Get the raw data
     * @param buf_size
     * @return
    */
    inline uint8_t* toBytes(size_t& buf_size) const {
        buf_size = sizeof(CSIStandardData);
        return reinterpret_cast<uint8_t*>(const_cast<CSIStandardData*>(&data));
    };

    /**
     * @brief Access the CSI matrix data directly
     * @return
     */
    inline const int32_t* matrix() {
        // Convert the CSI matrix to an int32_t array with one dimension
        auto* csi_matrix_ptr = reinterpret_cast<int32_t*>(data.csi_matrix);

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
        return &data.csi_matrix[ntr][nrx][subcarrier];
    };


    /**
     * @brief Set the CSI matrix data by given index
     * @param ntr
     * @param nrx
     * @param subcarrier
     * @param value
     */
    inline void set(int ntr, int nrx, int subcarrier, complex* value) {
        data.csi_matrix[ntr][nrx][subcarrier] = *value;
    };
};


#endif // __STANDARD_CSI_DATA_PROCESSING_H__