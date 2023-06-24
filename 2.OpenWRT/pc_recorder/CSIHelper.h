//
// Created by Orlando Chen on 2023/06/12.
//

#ifndef DENSITYDETECTION_CSIHELPER_H
#define DENSITYDETECTION_CSIHELPER_H

#include <cstdlib>

typedef struct
{
    int real;
    int imag;
} COMPLEX;

typedef struct
{
    u_int16_t   data_len;       // length of field
    u_int64_t   timestamp;      // timestamp of packet capture
    u_int16_t   csi_len;        // length of csi data in bytes
    u_int16_t   channel;        // wireless channel, represents in Hz
    u_int8_t    err_info;       // error code
    u_int8_t    noise_floor;    // noise level
    u_int8_t    rate;           // transmission rate
    u_int8_t    bandWidth;      // bandwidth of the channel, (0->20MHz,1->40MHz)
    u_int8_t    num_tones;      // number of tones
    u_int8_t    nr;             // umber of receiving antenna
    u_int8_t    nc;             // number of transmitting antenna
    u_int8_t    rssi;           // rx frame RSSI
    u_int8_t    rssi_0;         // rx frame RSSI [ctl, chain 0]
    u_int8_t    rssi_1;         // rx frame RSSI [ctl, chain 1]
    u_int8_t    rssi_2;         // rx frame RSSI [ctl, chain 2]
    u_int16_t   payload_len;    // payload length (bytes)
    COMPLEX     *csi_matrix;    // pointer to payload
} csi_struct;

extern bool  is_big_endian();

//int   read_csi_buf(unsigned char* buf_addr,int fd, int BUFSIZE);

extern void  record_status(unsigned char* buf_addr,
                           int cnt,
                           csi_struct* csi_status);

extern void  record_csi_payload(unsigned char* buf_addr,
                                csi_struct* csi_status,
                                unsigned char* data_buf,
                                COMPLEX(* csi_buf)[3][114]);

//void  porcess_csi(unsigned char* data_buf, csi_struct* csi_status,COMPLEX(* csi_buf)[3][114]);

#endif //DENSITYDETECTION_CSIHELPER_H
