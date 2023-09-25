#include <iostream>
#include <cstring>

#include "OpenWRT_v1.h"
#include "../local/LowBitsOperation.h"
#include "../bin/GeneralUtils.h"


OpenWrt_CSI_MetaInfo_V1  csi_meta;


void encode_openwrt_v1(int32_t* input, int32_t data_len, uint8_t* output) {

    // Extracted data from the buffer
    uint32_t buffer = 0;
    int32_t buf_bits_left = 0;
    uint32_t extracted = 0;

    // bytes index
    int32_t bytes_idx = 0;

    // Now, we can encode the data
    for (int data_idx = 0; data_idx < data_len; data_idx++) {

        // Clamp the value to 10 bits
        uint32_t clamp_val = input[data_idx] & 0x3FF;

        // Merge the bits into the buffer
        // Suppose the buffer is 0x11, and the clamp_val is 0x22, then the buffer will be 0x2211
        buffer = merge_little_endian(buffer, buf_bits_left, clamp_val, 10);

        // bits left plus 10
        buf_bits_left += 10;

        // If the bits left is greater than 8, then we can write the data to the buffer
        while (buf_bits_left >= 8) {

            // Extract the data from the buffer
            split_bits_little_endian(buffer, 8, extracted, buffer);

            // Write the data to the buffer
            output[bytes_idx++] = extracted;

            // bits left minus 8
            buf_bits_left -= 8;
        }
    }
}


void decode_openwrt_v1(uint8_t* input, int32_t bytes_len, int32_t* output) {

    // buffer index
    int32_t bytes_idx = 0;

    // Get the data by little endian
    auto h_data = read_16bits_by_little_endian(input, bytes_idx);
    uint32_t current_data = h_data;
    int32_t bits_left = 16;
    uint32_t extracted;
    int32_t number;

    // data index
    int32_t data_idx = 0;

    // Now, we can decode the data
    while (bytes_idx <= bytes_len) {
        if ((bits_left - 10) < 0) {
            // uint8_t buf[2] = { 0xCD, 0xAB }; ==> 0xABCD
            h_data = read_16bits_by_little_endian(input, bytes_idx);

            // 0xABCD + 0x123 ==> 0xABCD00 + 0x123 ==> 0xABC123
            current_data += h_data << bits_left;

            // bits added by 16
            bits_left += 16;
        }

        // 0xABC123 & 0x3FF ==> 0x123
        split_bits_little_endian(current_data, 10, extracted, current_data);

        // If the number is negative, then we need to convert it to negative
        number = convert_negative_data(extracted, 10);

        // bits left minus 10
        bits_left -= 10;

        // TODO: do something with the number
        output[data_idx++] = number;
    }
}


const OpenWrt_CSI_MetaInfo_V1* get_csi_metadata(const unsigned char* buf_addr, int cnt) {
    int32_t idx = 0;

    //if (is_big_endian()) {
    //    csi_meta.timestamp = read_64bits_by_big_endian(buf_addr, idx);
    //    csi_meta.csi_len = read_16bits_by_big_endian(buf_addr, idx);
    //    csi_meta.channel = read_16bits_by_big_endian(buf_addr, idx);
    //    csi_meta.buf_len = (buf_addr[cnt - 2] << 8) | buf_addr[cnt - 1];
    //    csi_meta.payload_len = (buf_addr[CSI_META_LEN] << 8) | buf_addr[CSI_META_LEN + 1];
    //} else {
    //    csi_meta.timestamp = read_64bits_by_little_endian(buf_addr, idx);
    //    csi_meta.csi_len = read_16bits_by_little_endian(buf_addr, idx);
    //    csi_meta.channel = read_16bits_by_little_endian(buf_addr, idx);
    //    csi_meta.buf_len = (buf_addr[cnt - 1] << 8) | buf_addr[cnt - 2];
    //    csi_meta.payload_len = (buf_addr[CSI_META_LEN + 1] << 8) | buf_addr[CSI_META_LEN];
    //}

    csi_meta.timestamp = read_64bits_by_big_endian(buf_addr, idx);
    csi_meta.csi_len = read_16bits_by_big_endian(buf_addr, idx);
    csi_meta.channel = read_16bits_by_big_endian(buf_addr, idx);
    csi_meta.buf_len = (buf_addr[cnt - 2] << 8) | buf_addr[cnt - 1];
    csi_meta.payload_len = (buf_addr[CSI_META_LEN] << 8) | buf_addr[CSI_META_LEN + 1];

    csi_meta.physical_err = buf_addr[12];
    csi_meta.noise = buf_addr[13];
    csi_meta.rate = buf_addr[14];
    csi_meta.bandwidth = buf_addr[15];
    csi_meta.num_tones = buf_addr[16] > 114 ? 114 : buf_addr[16];
    csi_meta.nr = buf_addr[17] > 3 ? 3 : buf_addr[17];
    csi_meta.nc = buf_addr[18] > 3 ? 3 : buf_addr[18];

    csi_meta.rssi = buf_addr[19];
    csi_meta.rssi_0 = buf_addr[20];
    csi_meta.rssi_1 = buf_addr[21];
    csi_meta.rssi_2 = buf_addr[22];

    return &csi_meta;
}


void gen_fake_data(uint8_t *buf, size_t& buf_size) {

    if (buf != nullptr) {

        /*
         * timestamp (8 bytes): 从buf_addr[0]到buf_addr[7]。这8个字节被解释为一个64位整数，其中buf_addr[0]是最高有效字节，buf_addr[7]是最低有效字节。
         * csi_len (2 bytes): 从buf_addr[8]到buf_addr[9]。这2个字节被解释为一个16位整数。
         * channel (2 bytes): 从buf_addr[10]到buf_addr[11]。这2个字节被解释为一个16位整数。
         * physical_err (1 byte): 位于buf_addr[12]。
         * noise (1 byte): 位于buf_addr[13]。
         * rate (1 byte): 位于buf_addr[14]。
         * bandwidth (1 byte): 位于buf_addr[15]。
         * num_tones (1 byte): 位于buf_addr[16]。
         * nr (1 byte): 位于buf_addr[17]。
         * nc (1 byte): 位于buf_addr[18]。
         * rssi (1 byte): 位于buf_addr[19]。
         * rssi_0 (1 byte): 位于buf_addr[20]。
         * rssi_1 (1 byte): 位于buf_addr[21]。
         * rssi_2 (1 byte): 位于buf_addr[22]。
         * payload_len (2 bytes): 从buf_addr[23]到buf_addr[24]。虽然名为"payload_len"，但它位于CSI_META_LEN后面，所以实际上它是buf_addr的第24和25个字节。
         * buf_len (2 bytes): 最后2个字节。它的位置基于cnt，这意味着它位于buf_addr的末尾。
         */

        // Assign the information of the fake data
        csi_meta.timestamp = timestamp();
        csi_meta.channel = 2462;
        csi_meta.payload_len = 40;
        csi_meta.physical_err = 0;
        csi_meta.noise = 0;
        csi_meta.rate = 151;
        csi_meta.bandwidth = 0;
        csi_meta.num_tones = DEFAULT_TONES; // Number of tones or subcarriers
        csi_meta.nr = DEFAULT_RX; // Receive antenna number
        csi_meta.nc = DEFAULT_TX; // Transmit antenna number
        csi_meta.rssi = 51;
        csi_meta.rssi_0 = 46;
        csi_meta.rssi_1 = 42;
        csi_meta.rssi_2 = 47;
        // Till now, there are 23 bytes in the meta_info
        // After the 23rd byte, we have the payload_len
        // And the 24th and 25th bytes are the payload_len
        // So the CSI matrix starts from the 26th byte

        // Assign the csi length
        csi_meta.csi_len = csi_meta.nr * csi_meta.nc * csi_meta.num_tones * 2 * 10 / 8;

        // Now we copy the meta_info to the buffer
        std::memcpy(buf, &csi_meta.timestamp, sizeof(uint64_t));
        std::memcpy(buf + 8, &csi_meta.csi_len, sizeof(uint16_t));
        std::memcpy(buf + 10, &csi_meta.channel, sizeof(uint16_t));
        std::memcpy(buf + 12, &csi_meta.physical_err, sizeof(uint8_t));
        std::memcpy(buf + 13, &csi_meta.noise, sizeof(uint8_t));
        std::memcpy(buf + 14, &csi_meta.rate, sizeof(uint8_t));
        std::memcpy(buf + 15, &csi_meta.bandwidth, sizeof(uint8_t));
        std::memcpy(buf + 16, &csi_meta.num_tones, sizeof(uint8_t));
        std::memcpy(buf + 17, &csi_meta.nr, sizeof(uint8_t));
        std::memcpy(buf + 18, &csi_meta.nc, sizeof(uint8_t));
        std::memcpy(buf + 19, &csi_meta.rssi, sizeof(uint8_t));
        std::memcpy(buf + 20, &csi_meta.rssi_0, sizeof(uint8_t));
        std::memcpy(buf + 21, &csi_meta.rssi_1, sizeof(uint8_t));
        std::memcpy(buf + 22, &csi_meta.rssi_2, sizeof(uint8_t));
        std::memcpy(buf + 23, &csi_meta.payload_len, sizeof(uint16_t));

#if 0
        //// Print the buf ////
        // Print the first 8 bytes
        std::cout << "timestamp " << std::dec << csi_meta.timestamp << " ----- ";
        for (int i = 0; i < 8; i++) {
            // Decimal ----- Hex
            std::cout << std::hex << "0x" << (int) buf[i] << " ";
        }
        std::cout << std::endl;

        // Print the 9th and 10th bytes
        std::cout << "csi_len " << std::dec << csi_meta.csi_len << " ----- ";
        for (int i = 8; i < 10; i++) {
            // Decimal ----- Hex
            std::cout << std::hex << "0x" << (int) buf[i] << " ";
        }
        std::cout << std::endl;

        // Print the 11th and 12th bytes
        std::cout << "channel " << std::dec << csi_meta.channel << " ----- ";
        for (int i = 10; i < 12; i++) {
            // Decimal ----- Hex
            std::cout << std::hex << "0x" << (int) buf[i] << " ";
        }
        std::cout << std::endl;

        // Print the 13th byte
        std::cout << "physical_err " << std::dec << (int) csi_meta.physical_err << " ----- ";
        std::cout << std::hex << "0x" << (int) buf[12] << std::endl;

        // Print the 14th byte
        std::cout << "noise " << std::dec << (int) csi_meta.noise << " ----- ";
        std::cout << std::hex << "0x" << (int) buf[13] << std::endl;

        // Print the 15th byte
        std::cout << "rate " << std::dec << (int) csi_meta.rate << " ----- ";
        std::cout << std::hex << "0x" << (int) buf[14] << std::endl;

        // Print the 16th byte
        std::cout << "bandwidth " << std::dec << (int) csi_meta.bandwidth << " ----- ";
        std::cout << std::hex << "0x" << (int) buf[15] << std::endl;

        // Print the 17th byte
        std::cout << "num_tones " << std::dec << (int) csi_meta.num_tones << " ----- ";
        std::cout << std::hex << "0x" << (int) buf[16] << std::endl;

        // Print the 18th byte
        std::cout << "nr " << std::dec << (int) csi_meta.nr << " ----- ";
        std::cout << std::hex << "0x" << (int) buf[17] << std::endl;

        // Print the 19th byte
        std::cout << "nc " << std::dec << (int) csi_meta.nc << " ----- ";
        std::cout << std::hex << "0x" << (int) buf[18] << std::endl;

        // Print the 20th byte
        std::cout << "rssi " << std::dec << (int) csi_meta.rssi << " ----- ";
        std::cout << std::hex << "0x" << (int) buf[19] << std::endl;

        // Print the 21st byte
        std::cout << "rssi_0 " << std::dec << (int) csi_meta.rssi_0 << " ----- ";
        std::cout << std::hex << "0x" << (int) buf[20] << std::endl;

        // Print the 22nd byte
        std::cout << "rssi_1 " << std::dec << (int) csi_meta.rssi_1 << " ----- ";
        std::cout << std::hex << "0x" << (int) buf[21] << std::endl;

        // Print the 23rd byte
        std::cout << "rssi_2 " << std::dec << (int) csi_meta.rssi_2 << " ----- ";
        std::cout << std::hex << "0x" << (int) buf[22] << std::endl;

        // Print the 24th and 25th bytes
        std::cout << "payload_len " << std::dec << csi_meta.payload_len << " ----- ";
        for (int i = 23; i < 25; i++) {
            // Decimal ----- Hex
            std::cout << std::hex << "0x" << (int) buf[i] << " ";
        }
        std::cout << std::endl;
#endif

        // Now we prepare the csi matrix
        const size_t csi_matrix_size = DEFAULT_TX * DEFAULT_RX * DEFAULT_TONES * 2;
        int32_t csi_matrix_1d_array[csi_matrix_size] = {0};

        // Convert the csi matrix to the 3D matrix
        int32_t (*csi_matrix_ptr)[DEFAULT_RX][DEFAULT_TONES][2] =
                reinterpret_cast<int32_t(*)[DEFAULT_RX][DEFAULT_TONES][2]>(csi_matrix_1d_array);

        // Now we generate the fake csi matrix
        for(int k = 0;k < DEFAULT_TONES;k++) {
            /* loop for each tx antenna */
            for (int tx = 0; tx < DEFAULT_TX; tx++) {
                /* loop for each rx antenna */
                for (int rx = 0; rx < DEFAULT_RX; rx++) {

                    // Calculate the index
                    auto index = 2 * (tx * DEFAULT_RX * DEFAULT_TONES + rx * DEFAULT_TONES + k);

                    // Modulus with 0x1FF to avoid overflow
                    index = index % 0x1FF;

                    // Set the real part
                    csi_matrix_ptr[tx][rx][k][0] = index % 250;
                    csi_matrix_ptr[tx][rx][k][1] = -(index + 1);

#if 0
                    // Print the csi matrix
                    std::cout << "CSI[" << tx << "][" << rx << "][" << k << "] " << csi_matrix_ptr[tx][rx][k][0]
                              << " " << csi_matrix_ptr[tx][rx][k][1] << std::endl;
#endif
                }
            }
        }

        // Create a buffer to store the csi matrix in compressed form
        uint8_t csi_compressed_buf[csi_matrix_size * 10 / 8] = {0};

        // Use encode_csi to encode the csi matrix
        encode_openwrt_v1(csi_matrix_1d_array, csi_matrix_size, csi_compressed_buf);

        // Copy the csi matrix to the buffer
        std::memcpy(buf + 25, csi_compressed_buf, csi_matrix_size * 10 / 8);

        // 40 bytes for the fake payload
        uint8_t fake_payload[40] = {0};

        // Copy the fake payload to the buffer
        std::memcpy(buf + 25 + csi_matrix_size * 10 / 8, fake_payload, 40);

        // Calculate the buffer length
        csi_meta.buf_len = 25 + csi_matrix_size * 10 / 8 + 40 + 2;

        // Copy the buffer length to the buffer
        std::memcpy(buf + 25 + csi_matrix_size * 10 / 8 + 40, &csi_meta.buf_len, sizeof(uint16_t));

        // Update the buffer size
        buf_size = csi_meta.buf_len;

    } else {
        std::cerr << "The buffer is nullptr" << std::endl;
        exit(EXIT_FAILURE);
    }
};

#if 1  // Enable the old version

complex csi_matrix[3][3][114] = { 0 };  // <=== Old version

int bit_convert(int data, int maxbit)
{
    if ( data & (1 << (maxbit - 1)))
    {
        /* negative */
        data -= (1 << maxbit);
    }
    return data;
}

void fill_csi_matrix(uint8_t* csi_addr, int8_t nr, int8_t nc, int8_t num_tones) {

    u_int8_t k;
    u_int8_t bits_left, nr_idx, nc_idx;
    u_int32_t bitmask, idx, current_data, h_data, h_idx;
    int real,imag;
    
    /* init bits_left
     * we process 16 bits at a time*/
    bits_left = 16;

    /* according to the h/w, we have 10 bit resolution
     * for each real and imag value of the csi matrix H
     */
    bitmask = (1 << 10) - 1;
    idx = h_idx = 0;
    /* get 16 bits for processing */
    h_data = csi_addr[idx++];
    h_data += (csi_addr[idx++] << 8);
    current_data = h_data & ((1 << 16) -1);

    /* loop for every subcarrier */
    for(k = 0;k < num_tones;k++){
        /* loop for each tx antenna */
        for(nc_idx = 0;nc_idx < nc;nc_idx++){
            /* loop for each rx antenna */
            for(nr_idx = 0;nr_idx < nr;nr_idx++){
                /* bits number less than 10, get next 16 bits */
                if((bits_left - 10) < 0){
                    h_data = csi_addr[idx++];
                    h_data += (csi_addr[idx++] << 8);
                    current_data += h_data << bits_left;
                    bits_left += 16;
                }

                imag = current_data & bitmask;
                imag = bit_convert(imag, 10);
                //printf("imag is: %d | ",imag);
                csi_matrix[nr_idx][nc_idx][k].imag = imag;
                //printf("imag is: %d | ",csi_matrix[nr_idx][nc_idx][k].imag);


                bits_left -= 10;
                current_data = current_data >> 10;

                /* bits number less than 10, get next 16 bits */
                if((bits_left - 10) < 0){
                    h_data = csi_addr[idx++];
                    h_data += (csi_addr[idx++] << 8);
                    current_data += h_data << bits_left;
                    bits_left += 16;
                }

                real = current_data & bitmask;
                real = bit_convert(real,10);
                //printf("real is: %d |",real);
                csi_matrix[nr_idx][nc_idx][k].real = real;
                //printf("real is: %d \n",csi_matrix[nr_idx][nc_idx][k].real);

                bits_left -= 10;
                current_data = current_data >> 10;
            }
        }
    }
}
#endif