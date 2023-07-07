#include <cstdint>

#include "CSIHelper.h"

#define CSI_META_LEN 23

COMPLEX     csi_matrix[3][3][114];
csi_struct  csi_meta;


bool is_big_endian(){
    unsigned int test = 0xFF000000;

    if ( *((unsigned char*)(&test)) == 0xFF)
    {
        return true;
    } else {
        return false;
    }
}


csi_struct* get_csi_metadata(const unsigned char* buf_addr, int cnt) {
    uint16_t csi_len;
    uint16_t channel;
    uint16_t buf_len;
    uint16_t payload_len;

    if (is_big_endian()) {
        csi_len = (buf_addr[8] << 8) | buf_addr[9];
        channel = (buf_addr[10] << 8) | buf_addr[11];
        buf_len = (buf_addr[cnt - 2] << 8) | buf_addr[cnt - 1];
        payload_len = (buf_addr[CSI_META_LEN] << 8) | buf_addr[CSI_META_LEN + 1];
    } else {
        csi_len = (buf_addr[9] << 8) | buf_addr[8];
        channel = (buf_addr[11] << 8) | buf_addr[10];
        buf_len = (buf_addr[cnt - 1] << 8) | buf_addr[cnt - 2];
        payload_len = (buf_addr[CSI_META_LEN + 1] << 8) | buf_addr[CSI_META_LEN];
    }

    csi_meta.tstamp = 0;
    csi_meta.csi_len = csi_len;
    csi_meta.channel = channel;
    csi_meta.buf_len = buf_len;
    csi_meta.payload_len = payload_len;

    csi_meta.phyerr = buf_addr[12];
    csi_meta.noise = buf_addr[13];
    csi_meta.rate = buf_addr[14];
    csi_meta.chanBW = buf_addr[15];
    csi_meta.num_tones = buf_addr[16] > 114 ? 114 : buf_addr[16];
    csi_meta.nr = buf_addr[17] > 3 ? 3 : buf_addr[17];
    csi_meta.nc = buf_addr[18] > 3 ? 3 : buf_addr[18];

    csi_meta.rssi = buf_addr[19];
    csi_meta.rssi_0 = buf_addr[20];
    csi_meta.rssi_1 = buf_addr[21];
    csi_meta.rssi_2 = buf_addr[22];

    return &csi_meta;
}


int bit_convert(int data, int max_bit)
{
    if ( data & (1 << (max_bit - 1)))
    {
        /* negative */
        data -= (1 << max_bit);
    }
    return data;
}


COMPLEX get_complex_data(const unsigned char* buf,
                         int& bits_left,
                         uint32_t& current_data)
{
    uint8_t bitmask = (1 << 10) - 1;
    int imag, real;

    if (bits_left < 10) {
        uint16_t h_data = buf[0] | (buf[1] << 8);
        current_data += h_data << bits_left;
        bits_left += 16;
        buf += 2;
    }

    imag = current_data & bitmask;
    imag = (imag & (1 << 9)) ? imag - (1 << 10) : imag;
    bits_left -= 10;
    current_data >>= 10;

    if (bits_left < 10) {
        uint16_t h_data = buf[0] | (buf[1] << 8);
        current_data += h_data << bits_left;
        bits_left += 16;
        buf += 2;
    }

    real = current_data & bitmask;
    real = (real & (1 << 9)) ? real - (1 << 10) : real;
    bits_left -= 10;
    current_data >>= 10;

    return {real, imag};
}


COMPLEX* get_csi_matrix(const unsigned char* buf, csi_struct* csi_status) {

    int bits_left = 16;
    uint32_t current_data = buf[0] | (buf[1] << 8);
    buf += 2;

    for (uint8_t k = 0; k < csi_status->num_tones; k++) {
        for (uint8_t nc_idx = 0; nc_idx < csi_status->nc; nc_idx++) {
            for (uint8_t nr_idx = 0; nr_idx < csi_status->nr; nr_idx++) {
                COMPLEX data = get_complex_data(buf, bits_left, current_data);
                csi_matrix[nr_idx][nc_idx][k].real = data.real;
                csi_matrix[nr_idx][nc_idx][k].imag = data.imag;
            }
        }
    }

    return &csi_matrix[0][0][0];
};