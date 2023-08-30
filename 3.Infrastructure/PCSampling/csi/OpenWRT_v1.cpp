#include <cstdint>

#include "CSIHelper.h"

#define CSI_META_LEN 23


CSIComplex   csi_matrix[3][3][114];
OpenWrt_CSI_MetaInfo_V1  csi_meta;


OpenWrt_CSI_MetaInfo_V1* get_csi_metadata(const unsigned char* buf_addr, int cnt) {
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

int bit_convert(int data, int maxbit)
{
   if ( data & (1 << (maxbit - 1)))
   {
       /* negative */
       data -= (1 << maxbit);
   }
   return data;
}


CSIComplex* fill_csi_matrix(u_int8_t* csi_addr, int nr, int nc, int num_tones) {

   u_int8_t k;
   u_int8_t bits_left, nr_idx, nc_idx;
   u_int32_t bitmask, idx, current_data, h_data;
   int real,imag;
   /* init bits_left
    * we process 16 bits at a time*/
   bits_left = 16;

   /* according to the h/w, we have 10 bit resolution
    * for each real and imag value of the csi matrix H
    */
//    bitmask = (1 << 10) - 1;  // 1023 in decimal, or 1111111111 in binary, or 0x3FF in hex
   idx = 0;

   /* get 16 bits for processing */
//    h_data = csi_addr[idx++];
//    h_data += (csi_addr[idx++] << 8); // h_data = 0, 0, csi_addr[1], csi_addr[0]
    h_data = csi_addr[idx] | (csi_addr[idx + 1] << 8); idx += 2; // this should be the same as the above two lines

//    current_data = h_data & ((1 << 16) -1); // this is equivalent to the following line
    // current_data = h_data & 0xFFFF; // 65535 in decimal, or 1111111111111111 in binary, or 0xFFFF in hex
    // thus, we can simply use the following line
    current_data = h_data;


   /* loop for every subcarrier */
   for(k = 0;k < num_tones;k++){
       /* loop for each tx antenna */
       for(nc_idx = 0;nc_idx < nc;nc_idx++){
           /* loop for each rx antenna */
           for(nr_idx = 0;nr_idx < nr;nr_idx++){


               /* bits number less than 10, get next 16 bits */
               if((bits_left - 10) < 0){
                // Improve the performance by removing the following two lines
                //    h_data = csi_addr[idx++];
                //    h_data += (csi_addr[idx++] << 8);
                    h_data = csi_addr[idx] | (csi_addr[idx + 1] << 8); idx += 2; // this should be the same as the above two lines
                   
                   
                   current_data += h_data << bits_left; 
                   // this line means that we shift h_data to the left by bits_left bits, and then add it to current_data
                    // suppose h_data is 0000 0000 1010 1010, and bits_left is 6, then the result is 0000 0000 1010 1010 0000 00
                    // that is to say, 0010 1010 1000 0000


                   bits_left += 16;
               }

            //    imag = current_data & bitmask; // bitmask is unnecessary here, so we can remove it
                // bitmask is 1111111111 in binary. Considering the above example, 0010 1010 1000 0000 & 1111111111 = 0010 1010 1000 0000
                imag = current_data; // Data range is available from -512 to 511


               imag = bit_convert(imag, 10);
                // If the most significant bit is 1, then it is a negative number. However, the most significant bit is 0 in the above example.

               //printf("imag is: %d | ",imag);
               csi_matrix[nr_idx][nc_idx][k].imag = imag;
               //printf("imag is: %d | ",csi_matrix[nr_idx][nc_idx][k].imag);


               bits_left -= 10; 
               // bits_left is 6 in the above example

               current_data = current_data >> 10;
               // Considering the above example, 0010 1010 1000 0000 >> 10 = 0010 10 

               /* bits number less than 10, get next 16 bits */
               if((bits_left - 10) < 0){
                 // Improve the performance by removing the following two lines
                //    h_data = csi_addr[idx++];
                //    h_data += (csi_addr[idx++] << 8);
                    h_data = csi_addr[idx] | (csi_addr[idx + 1] << 8); idx += 2; // this should be the same as the above two lines
                    // Suppose the next two bytes are 1111 1000 and 0000 1111, then h_data is 0000 1111 1111 1000


                   current_data += h_data << bits_left;
                    // Suppose current_data is 0010 10, then current_data += h_data << bits_left is 
                    // current_data = 0000 1010 + 0000 1111 1111 1000 << 6 = 0000 1010 + 1111 1100 0000 0000 = 1111 1100 0000 1010

                   bits_left += 16;
               }

            //    real = current_data & bitmask; // bitmask is unnecessary here, so we can remove it
                real = current_data;


               real = bit_convert(real,10); // sign check
                // If the most significant bit is 1, then it is a negative number. However, the most significant bit is 0 in the above example.

               //printf("real is: %d |",real);
               csi_matrix[nr_idx][nc_idx][k].real = real;
               //printf("real is: %d \n",csi_matrix[nr_idx][nc_idx][k].real);

               bits_left -= 10;
               current_data = current_data >> 10;



                // From the above sample, we can say that every 8 bytes are used to represent a real or an imaginary number.
           }
       }
   }

   return csi_matrix;
}
