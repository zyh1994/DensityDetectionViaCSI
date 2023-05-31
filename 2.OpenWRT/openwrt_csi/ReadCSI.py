TONE_40M = 114
BITS_PER_BYTE = 8
BITS_PER_SYMBOL = 10
BITS_PER_COMPLEX_SYMBOL = 2 * BITS_PER_SYMBOL


class Complex:
    def __init__(self, real, imag):
        self.real = real
        self.imag = imag

def signbit_convert(data, maxbit):
    if data & (1 << (maxbit - 1)):
        data -= (1 << maxbit)
    return data

def read_csi(local_h, nr, nc, num_tones):
    bitmask = (1 << BITS_PER_SYMBOL) - 1
    idx = h_idx = 0
    h_data = local_h[idx]
    idx += 1
    h_data += (local_h[idx] << BITS_PER_BYTE)
    idx += 1
    current_data = h_data & ((1 << 16) - 1)

    csi_real = []
    csi_imag = []

    bits_left = 16
    for k in range(num_tones):
        for nc_idx in range(nc):
            for nr_idx in range(nr):
                if bits_left - BITS_PER_SYMBOL < 0:
                    h_data = local_h[idx]
                    idx += 1
                    h_data += (local_h[idx] << BITS_PER_BYTE)
                    idx += 1
                    current_data += h_data << bits_left
                    bits_left += 16

                imag = current_data & bitmask
                imag = signbit_convert(imag, BITS_PER_SYMBOL)
                csi_imag.append(float(imag))
                bits_left -= BITS_PER_SYMBOL
                current_data = current_data >> BITS_PER_SYMBOL

                if bits_left - BITS_PER_SYMBOL < 0:
                    h_data = local_h[idx]
                    idx += 1
                    h_data += (local_h[idx] << BITS_PER_BYTE)
                    idx += 1
                    current_data += h_data << bits_left
                    bits_left += 16

                real = current_data & bitmask
                real = signbit_convert(real, BITS_PER_SYMBOL)
                csi_real.append(float(real))
                bits_left -= BITS_PER_SYMBOL
                current_data = current_data >> BITS_PER_SYMBOL

    return csi_real, csi_imag

def main():
    local_h = []  # initialize your local_h list
    nr = 2
    nc = 3
    num_tones = 4

    csi_real, csi_imag = read_csi(local_h, nr, nc, num_tones)

    # Print the read CSI data
    print("CSI Real:")
    print(csi_real)

    print("CSI Imaginary:")
    print(csi_imag)

if __name__ == "__main__":
    main()
