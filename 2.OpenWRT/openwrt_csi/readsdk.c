/*
 * =====================================================================================
 *       Filename:  read_csi.c
 *
 *    Description:  read csi for C 
 *        Version:  1.0
 *
 *         Author:  Yaxiong Xie
 *         Email :  <xieyaxiongfly@gmail.com>
 *   Organization:  WNADS group @ Nanyang Technological University
 *
 *   Copyright (c)  WANDS group @ Nanyang Technological University
 * =====================================================================================
 */
#include <stdio.h>

#define TONE_40M 114
#define BITS_PER_BYTE 8
#define BITS_PER_COMPLEX_SYMBOL (2 * BITS_PER_SYMBOL)
#define BITS_PER_SYMBOL      10

typedef struct
{
    int real;
    int imag;
} COMPLEX;

int signbit_convert(int data, int maxbit)
{
    if (data & (1 << (maxbit - 1))) 
    { /*  negative */
        data -= (1 << maxbit);
    }
    return data;
}

void read_csi(unsigned char *local_h, int nr, int nc, int num_tones, double *csi_real, double *csi_imag)
{
    unsigned int  bitmask, idx, current_data;
    unsigned int  h_data, h_idx;
    
    int  k;
    int  real, imag;
    int  bits_left, nc_idx, nr_idx;
  
    bits_left = 16; /* process 16 bits at a time */

    /* 10 bit resoluation for H real and imag */
    bitmask = (1 << BITS_PER_SYMBOL) - 1;
    idx = h_idx = 0;
    h_data = local_h[idx++];
    h_data += (local_h[idx++] << BITS_PER_BYTE);
    current_data = h_data & ((1 << 16) - 1); /* get 16 LSBs first */
    
    for (k = 0; k < num_tones; k++) {
        for (nc_idx = 0; nc_idx < nc; nc_idx++) {
            for (nr_idx = 0; nr_idx < nr; nr_idx++) {
                if ((bits_left - BITS_PER_SYMBOL) < 0) {
                    /* get the next 16 bits */
                    h_data = local_h[idx++];
                    h_data += (local_h[idx++] << BITS_PER_BYTE);
                    current_data += h_data << bits_left;
                    bits_left += 16;
                }
                imag = current_data & bitmask;
                
                imag = signbit_convert(imag, BITS_PER_SYMBOL);
		        *csi_imag = (double) imag;
		        ++csi_imag;
                bits_left -= BITS_PER_SYMBOL;
                /* shift out used bits */
                current_data = current_data >> BITS_PER_SYMBOL; 

                if ((bits_left - BITS_PER_SYMBOL) < 0) {
                    /* get the next 16 bits */
                    h_data = local_h[idx++];
                    h_data += (local_h[idx++] << BITS_PER_BYTE);
                    current_data += h_data << bits_left;
                    bits_left += 16;
                }
                real = current_data & bitmask;
                
                real = signbit_convert(real, BITS_PER_SYMBOL);
		        *csi_real = (double) real;
		        ++csi_real;
                bits_left -= BITS_PER_SYMBOL;
                
                /* shift out used bits */
                current_data = current_data >> BITS_PER_SYMBOL;
                
            }
        }
    }
}


/*
int main()
{
    unsigned char local_h[] = {}; // initialize your local_h array
    int nr = 2;
    int nc = 3;
    int num_tones = 4;

    const int csi_size = nr * nc * num_tones;
    double csi_real[csi_size];
    double csi_imag[csi_size];

    read_csi(local_h, nr, nc, num_tones, csi_real, csi_imag);

    printf("CSI Real:\n");
    for (int i = 0; i < csi_size; i++) {
        printf("%f ", csi_real[i]);
    }
    printf("\n");

    printf("CSI Imaginary:\n");
    for (int i = 0; i < csi_size; i++) {
        printf("%f ", csi_imag[i]);
    }
    printf("\n");

    return 0;
}
*/

