//
// Created by Orlando Chen on 2023/06/12.
//

#ifndef __OPENWRT_V1_H__
#define __OPENWRT_V1_H__

#include <cstdlib>
#include <cstdint>

typedef struct
{
    uint64_t timestamp;    /* h/w assigned time stamp */
    uint16_t csi_len;      /*  csi data length (bytes) */
    uint16_t channel;      /* wireless channel (represented in Hz)*/
    uint16_t buf_len;      /*  data length in buffer */
    uint16_t payload_len;  /*  payload length (bytes) */
    uint8_t  physical_err; /* phy error code (set to 0 if correct)*/
    uint8_t  noise;        /* noise floor (to be updated)*/
    uint8_t  rate;         /* transmission rate*/
    uint8_t  bandwidth;    /* channel bandwidth (0->20MHz,1->40MHz)*/
    uint8_t  num_tones;    /* number of tones (subcarriers) */
    uint8_t  nr;           /* number of receiving antenna*/
    uint8_t  nc;           /* number of transmitting antenna*/
    uint8_t  rssi;         /*  rx frame RSSI */
    uint8_t  rssi_0;       /*  rx frame RSSI [ctl, chain 0] */
    uint8_t  rssi_1;       /*  rx frame RSSI [ctl, chain 1] */
    uint8_t  rssi_2;       /*  rx frame RSSI [ctl, chain 2] */
} OpenWrt_CSI_MetaInfo_V1;


#ifndef OPENWRT_CSI_META_INFO_V1
#define OPENWRT_CSI_META_INFO_V1
#define DEFAULT_TX      3   // Transmitter
#define DEFAULT_RX      3   // Receiver
#define DEFAULT_TONES   116 // Number of subcarriers
#define CSI_META_LEN    23  // Length of CSI meta info
#endif

#include "StandardCSIDataStruct.h"


extern const OpenWrt_CSI_MetaInfo_V1*  get_csi_metadata(const unsigned char* buf, int buf_size);


extern void decode_openwrt_v1(uint8_t* input, int32_t bytes_len, int32_t* output);


extern void encode_openwrt_v1(int32_t* input, int32_t data_len, uint8_t* output);


extern void gen_fake_data(uint8_t *buf, size_t& buf_size);


#if 1  // Enable the old version
extern complex csi_matrix[3][3][114];
extern void fill_csi_matrix(uint8_t* buf, int8_t nr, int8_t nc, int8_t num_tones);
#endif

#endif //DENSITYDETECTION_CSIHELPER_H
