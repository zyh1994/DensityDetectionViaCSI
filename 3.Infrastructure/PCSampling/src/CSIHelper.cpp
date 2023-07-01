/*
 * =====================================================================================
 *      Filename:  ath9k_csi.c
 *
 *   Description:  basic csi processing fucntion
 *                 you can implement your own fucntion here
 *       Version:  1.0
 *
 *        Author:  Yaxiong Xie
 *        Email :  <xieyaxiongfly@gmail.com>
 *  Organization:  WANDS group @ Nanyang Technological University
 *
 *   Copyright (c) WANDS group @ Nanyang Technological University
 * =====================================================================================
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>

#include "../inc/CSIHelper.h"

#define csi_st_len 23

/*
 * Check if the current system is big endian
 */
bool is_big_endian(){
    unsigned int test = 0xFF000000;

    if ( *((unsigned char*)(&test)) == 0xFF)
    {
        return true;
    } else {
        return false;
    }
}

/*
 * Swap the bytes of the input integer
 */
int bit_convert(int data, int maxbit)
{
    if ( data & (1 << (maxbit - 1)))
    {
        /* negative */
        data -= (1 << maxbit);
    }
    return data;
}

/*
 * Read the CSI from the file
 */
void fill_csi_matrix(u_int8_t* csi_addr, int nr, int nc, int num_tones, COMPLEX(*csi_matrix)[3][114]) {
    u_int32_t bitmask = (1 << 10) - 1; // Bitmask for extracting 10 bits
    u_int32_t current_data = 0; // Current data being processed
    u_int8_t bits_left = 16; // Number of bits remaining in current_data
    u_int32_t h_data = 0; // Temporary storage for 16-bit data
    int real, imag; // Real and imaginary parts of the CSI matrix element

    // Loop for every subcarrier
    for (u_int8_t k = 0; k < num_tones; k++) {
        // Loop for each tx antenna
        for (u_int8_t nc_idx = 0; nc_idx < nc; nc_idx++) {
            // Loop for each rx antenna
            for (u_int8_t nr_idx = 0; nr_idx < nr; nr_idx++) {
                // If the number of bits remaining is less than 10, fetch next 16 bits from input buffer
                if (bits_left < 10) {
                    h_data = csi_addr[0] | (csi_addr[1] << 8);
                    csi_addr += 2; // Move the buffer pointer to the next 16-bit data
                    current_data |= h_data << bits_left;
                    bits_left += 16;
                }

                imag = current_data & bitmask; // Extract the lower 10 bits as the imaginary part
                imag = bit_convert(imag, 10); // Perform any necessary conversion on the extracted value
                csi_matrix[nr_idx][nc_idx][k].imag = imag; // Store the imaginary part in the CSI matrix

                bits_left -= 10;
                current_data >>= 10; // Shift right by 10 bits to prepare for the next extraction

                // If the number of bits remaining is less than 10, fetch next 16 bits from input buffer
                if (bits_left < 10) {
                    h_data = csi_addr[0] | (csi_addr[1] << 8);
                    csi_addr += 2; // Move the buffer pointer to the next 16-bit data
                    current_data |= h_data << bits_left;
                    bits_left += 16;
                }

                real = current_data & bitmask; // Extract the lower 10 bits as the real part
                real = bit_convert(real, 10); // Perform any necessary conversion on the extracted value
                csi_matrix[nr_idx][nc_idx][k].real = real; // Store the real part in the CSI matrix

                bits_left -= 10;
                current_data >>= 10; // Shift right by 10 bits to prepare for the next extraction
            }
        }
    }
}


/*
 * Read the CSI from the file
 */
int read_csi_buf(unsigned char* buf_addr,int fd, int BUFSIZE){
    int cnt;
    /* listen to the port
     * read when 1, a csi is reported from kernel
     *           2, time out
     */
    cnt = read(fd,buf_addr,BUFSIZE);
    if(cnt)
        return cnt;
    else
        return 0;
}

/*
 * Read the CSI from the file
 */
void record_status(unsigned char* buf_addr, int cnt, csi_struct* csi_status) {
    uint64_t tstamp = 0;
    uint16_t csi_len = 0;
    uint16_t channel = 0;
    uint16_t buf_len = 0;
    uint16_t payload_len = 0;

    if (is_big_endian()) {
        tstamp = ((uint64_t)buf_addr[0] << 56) | ((uint64_t)buf_addr[1] << 48) |
                 ((uint64_t)buf_addr[2] << 40) | ((uint64_t)buf_addr[3] << 32) |
                 ((uint64_t)buf_addr[4] << 24) | ((uint64_t)buf_addr[5] << 16) |
                 ((uint64_t)buf_addr[6] << 8) | buf_addr[7];
        csi_len = (buf_addr[8] << 8) | buf_addr[9];
        channel = (buf_addr[10] << 8) | buf_addr[11];
        buf_len = (buf_addr[cnt - 2] << 8) | buf_addr[cnt - 1];
        payload_len = (buf_addr[csi_st_len] << 8) | buf_addr[csi_st_len + 1];
    } else {
        tstamp = ((uint64_t)buf_addr[7] << 56) | ((uint64_t)buf_addr[6] << 48) |
                 ((uint64_t)buf_addr[5] << 40) | ((uint64_t)buf_addr[4] << 32) |
                 ((uint64_t)buf_addr[3] << 24) | ((uint64_t)buf_addr[2] << 16) |
                 ((uint64_t)buf_addr[1] << 8) | buf_addr[0];
        csi_len = (buf_addr[9] << 8) | buf_addr[8];
        channel = (buf_addr[11] << 8) | buf_addr[10];
        buf_len = (buf_addr[cnt - 1] << 8) | buf_addr[cnt - 2];
        payload_len = (buf_addr[csi_st_len + 1] << 8) | buf_addr[csi_st_len];
    }

    csi_status->tstamp = tstamp;
    csi_status->csi_len = csi_len;
    csi_status->channel = channel;
    csi_status->buf_len = buf_len;
    csi_status->payload_len = payload_len;

    csi_status->phyerr = buf_addr[12];
    csi_status->noise = buf_addr[13];
    csi_status->rate = buf_addr[14];
    csi_status->chanBW = buf_addr[15];
    csi_status->num_tones = buf_addr[16];
    csi_status->nr = buf_addr[17];
    csi_status->nc = buf_addr[18];

    csi_status->rssi = buf_addr[19];
    csi_status->rssi_0 = buf_addr[20];
    csi_status->rssi_1 = buf_addr[21];
    csi_status->rssi_2 = buf_addr[22];
}

/*
 * Record the CSI data to the data buffer
 */
//void record_csi_payload(unsigned char* buf_addr, csi_struct* csi_status, unsigned char* data_buf,
// COMPLEX(* csi_matrix)[3][114]){
//    int i;
//    int nr,nc,num_tones;
//    u_int8_t* csi_addr;
//    u_int16_t payload_len, csi_len;
//
//    nr          = csi_status->nr;
//    nc          = csi_status->nc;
//    num_tones   = csi_status->num_tones;
//    payload_len = csi_status->payload_len;
//    csi_len     = csi_status->csi_len;
//
//    /* record the data to the data buffer*/
//    for (i=1;i<=payload_len;i++){
//        //    printf("i is: %d \n",i);
//        data_buf[i-1] = buf_addr[csi_st_len + csi_len + i + 1];
//    }
//
//    /* extract the CSI and fill the complex matrix */
//    csi_addr = buf_addr + csi_st_len + 2;
//    fill_csi_matrix(csi_addr,nr,nc,num_tones, csi_matrix);
//}
