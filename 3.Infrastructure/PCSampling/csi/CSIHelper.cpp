#include <cstdint>

#include "CSIHelper.h"

#define CSI_META_LEN 23

//CSIComplex   csi_matrix[3][3][114];
CSIMetaInfo  csi_meta;


CSIMetaInfo* get_csi_metadata(const unsigned char* buf_addr, int cnt) {
    uint16_t csi_len;
    uint16_t channel;
    uint16_t buf_len;
    uint16_t payload_len;

//    if (is_big_endian()) {
        csi_len = (buf_addr[8] << 8) | buf_addr[9];
        channel = (buf_addr[10] << 8) | buf_addr[11];
        buf_len = (buf_addr[cnt - 2] << 8) | buf_addr[cnt - 1];
        payload_len = (buf_addr[CSI_META_LEN] << 8) | buf_addr[CSI_META_LEN + 1];
//    } else {
//        csi_len = (temp_buf[9] << 8) | temp_buf[8];
//        channel = (temp_buf[11] << 8) | temp_buf[10];
//        buf_len = (temp_buf[cnt - 1] << 8) | temp_buf[cnt - 2];
//        payload_len = (temp_buf[CSI_META_LEN + 1] << 8) | temp_buf[CSI_META_LEN];
//    }

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

//
//CSIComplex get_complex_data(const unsigned char* buf,
//                         int& bits_left,
//                         uint32_t& current_data)
//{
//    uint8_t bitmask = (1 << 10) - 1;
//    int imag, real;
//
//    if (bits_left < 10) {
//        uint16_t h_data = buf[0] | (buf[1] << 8);
//        current_data += h_data << bits_left;
//        bits_left += 16;
//        buf += 2;
//    }
//
//    imag = current_data & bitmask;
//    imag = (imag & (1 << 9)) ? imag - (1 << 10) : imag;
//    bits_left -= 10;
//    current_data >>= 10;
//
//    if (bits_left < 10) {
//        uint16_t h_data = buf[0] | (buf[1] << 8);
//        current_data += h_data << bits_left;
//        bits_left += 16;
//        buf += 2;
//    }
//
//    real = current_data & bitmask;
//    real = (real & (1 << 9)) ? real - (1 << 10) : real;
//    bits_left -= 10;
//    current_data >>= 10;
//
//    return {real, imag};
//}
//
//
//int bit_convert(int data, int maxbit)
//{
//    if ( data & (1 << (maxbit - 1)))
//    {
//        /* negative */
//        data -= (1 << maxbit);
//    }
//    return data;
//}
//
//
//void fill_csi_matrix(u_int8_t* csi_addr,
//                     int nr, int nc, int num_tones,
//                     CSIComplex(* csi_matrix)[3][114]){
//    u_int8_t k;
//    u_int8_t bits_left, nr_idx, nc_idx;
//    u_int32_t bitmask, idx, current_data, h_data;
//    int real,imag;
//    /* init bits_left
//     * we process 16 bits at a time*/
//    bits_left = 16;
//
//    /* according to the h/w, we have 10 bit resolution
//     * for each real and imag value of the csi matrix H
//     */
//    bitmask = (1 << 10) - 1;
//    idx = 0;
//    /* get 16 bits for processing */
//    h_data = csi_addr[idx++];
//    h_data += (csi_addr[idx++] << 8);
//    current_data = h_data & ((1 << 16) -1);
//
//    /* loop for every subcarrier */
//    for(k = 0;k < num_tones;k++){
//        /* loop for each tx antenna */
//        for(nc_idx = 0;nc_idx < nc;nc_idx++){
//            /* loop for each rx antenna */
//            for(nr_idx = 0;nr_idx < nr;nr_idx++){
//                /* bits number less than 10, get next 16 bits */
//                if((bits_left - 10) < 0){
//                    h_data = csi_addr[idx++];
//                    h_data += (csi_addr[idx++] << 8);
//                    current_data += h_data << bits_left;
//                    bits_left += 16;
//                }
//
//                imag = current_data & bitmask;
//                imag = bit_convert(imag, 10);
//                //printf("imag is: %d | ",imag);
//                csi_matrix[nr_idx][nc_idx][k].imag = imag;
//                //printf("imag is: %d | ",csi_matrix[nr_idx][nc_idx][k].imag);
//
//
//                bits_left -= 10;
//                current_data = current_data >> 10;
//
//                /* bits number less than 10, get next 16 bits */
//                if((bits_left - 10) < 0){
//                    h_data = csi_addr[idx++];
//                    h_data += (csi_addr[idx++] << 8);
//                    current_data += h_data << bits_left;
//                    bits_left += 16;
//                }
//
//                real = current_data & bitmask;
//                real = bit_convert(real,10);
//                //printf("real is: %d |",real);
//                csi_matrix[nr_idx][nc_idx][k].real = real;
//                //printf("real is: %d \n",csi_matrix[nr_idx][nc_idx][k].real);
//
//                bits_left -= 10;
//                current_data = current_data >> 10;
//            }
//        }
//
//    }
//}
//
//
//CSIComplex* get_csi_matrix(unsigned char* buf, CSIMetaInfo* csi_status) {
//
////    int bits_left = 16;
////    uint32_t current_data = buf[0] | (buf[1] << 8);
////    buf += 2;
////
////    for (uint8_t k = 0; k < csi_status->num_tones; k++) {
////        for (uint8_t nc_idx = 0; nc_idx < csi_status->nc; nc_idx++) {
////            for (uint8_t nr_idx = 0; nr_idx < csi_status->nr; nr_idx++) {
////                Complex data = get_complex_data(buf, bits_left, current_data);
////                csi_matrix[nr_idx][nc_idx][k].real = data.real;
////                csi_matrix[nr_idx][nc_idx][k].imag = data.imag;
////            }
////        }
////    }
//
//    /* extract the CSI and fill the complex matrix */
//    unsigned char* csi_addr = buf + CSI_META_LEN + 2;
//
//    fill_csi_matrix((u_int8_t*)csi_addr,
//                    csi_status->nr,
//                    csi_status->nc,
//                    csi_status->num_tones,
//                    csi_matrix);
//
//    return &csi_matrix[0][0][0];
//};