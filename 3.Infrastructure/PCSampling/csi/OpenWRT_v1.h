//
// Created by Orlando Chen on 2023/06/12.
//

#ifndef DENSITYDETECTION_CSIHELPER_H
#define DENSITYDETECTION_CSIHELPER_H

#include <cstdlib>

typedef struct
{
    u_int64_t tstamp;         /* h/w assigned time stamp */

    u_int16_t channel;        /* wireless channel (represented in Hz)*/
    u_int8_t  chanBW;         /* channel bandwidth (0->20MHz,1->40MHz)*/

    u_int8_t  rate;           /* transmission rate*/
    u_int8_t  nr;             /* number of receiving antenna*/
    u_int8_t  nc;             /* number of transmitting antenna*/
    u_int8_t  num_tones;      /* number of tones (subcarriers) */
    u_int8_t  noise;          /* noise floor (to be updated)*/

    u_int8_t  phyerr;          /* phy error code (set to 0 if correct)*/

    u_int8_t  rssi;         /*  rx frame RSSI */
    u_int8_t  rssi_0;       /*  rx frame RSSI [ctl, chain 0] */
    u_int8_t  rssi_1;       /*  rx frame RSSI [ctl, chain 1] */
    u_int8_t  rssi_2;       /*  rx frame RSSI [ctl, chain 2] */

    u_int16_t payload_len;  /*  payload length (bytes) */
    u_int16_t csi_len;      /*  csi data length (bytes) */
    u_int16_t buf_len;      /*  data length in buffer */

} OpenWrt_CSI_MetaInfo_V1;


typedef struct
{
    int real;
    int imag;
} CSIComplex;


extern const OpenWrt_CSI_MetaInfo_V1*  get_csi_metadata(const unsigned char* buf, int buf_size);


extern const CSIComplex* fill_csi_matrix(const unsigned char* buf, int nr, int nc, int num_tones);


#ifndef OPENWRT_CSI_INDEX_CALCULATION
#define OPENWRT_CSI_INDEX_CALCULATION

#define index(ptr, nc_idx, nr_idx, k, nc, num_tones) \
((ptr) + (nr_idx) * (nc) * (num_tones) + (nc_idx) * (num_tones) + (k))

#endif

#endif //DENSITYDETECTION_CSIHELPER_H
