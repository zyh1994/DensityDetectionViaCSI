

CSI_ST_LEN = 23


class CSIStatus:

    def __init__(self):
        self.tstamp = None
        self.csi_len = None
        self.channel = None
        self.buf_len = None
        self.payload_len = None
        self.phyerr = None
        self.noise = None
        self.rate = None
        self.chanBW = None
        self.num_tones = None
        self.nr = None
        self.nc = None
        self.rssi = None
        self.rssi_0 = None
        self.rssi_1 = None
        self.rssi_2 = None

    def __str__(self):
        return f"tstamp: {self.tstamp}, csi_len: {self.csi_len}, channel: {self.channel}, " \
                f"buf_len: {self.buf_len}, payload_len: {self.payload_len}, phyerr: {self.phyerr}, " \
                f"noise: {self.noise}, rate: {self.rate}, chanBW: {self.chanBW}, num_tones: {self.num_tones}, " \
                f"nr: {self.nr}, nc: {self.nc}, rssi: {self.rssi}, rssi_0: {self.rssi_0}, rssi_1: {self.rssi_1}, " \
                f"rssi_2: {self.rssi_2}"


def is_big_endian():
    # a = 0x1
    # b = a.to_bytes(4, 'little')[0]
    # if b == 0:
    #     return True
    return True


def record_status(buf_addr, cnt):

    csi_status = CSIStatus()

    if is_big_endian():
        # csi_status.tstamp = (
        #     (buf_addr[0] << 56) & 0x00000000000000ff | (buf_addr[1] << 48) & 0x000000000000ff00 |
        #     (buf_addr[2] << 40) & 0x0000000000ff0000 | (buf_addr[3] << 32) & 0x00000000ff000000 |
        #     (buf_addr[4] << 24) & 0x000000ff00000000 | (buf_addr[5] << 16) & 0x0000ff0000000000 |
        #     (buf_addr[6] << 8) & 0x00ff000000000000 | (buf_addr[7]) & 0xff00000000000000
        # )
        csi_status.csi_len = (buf_addr[8] << 8) & 0xff00 | (buf_addr[9] & 0x00ff)
        csi_status.channel = (buf_addr[10] << 8) & 0xff00 | (buf_addr[11] & 0x00ff)
        csi_status.buf_len = (buf_addr[cnt - 2] << 8) & 0xff00 | (buf_addr[cnt - 1] & 0x00ff)
        csi_status.payload_len = (
            (buf_addr[CSI_ST_LEN] << 8) & 0xff00 |
            (buf_addr[CSI_ST_LEN + 1]) & 0x00ff
        )
    else:
        # csi_status.tstamp = (
        #     (buf_addr[7] << 56) & 0x00000000000000ff | (buf_addr[6] << 48) & 0x000000000000ff00 |
        #     (buf_addr[5] << 40) & 0x0000000000ff0000 | (buf_addr[4] << 32) & 0x00000000ff000000 |
        #     (buf_addr[3] << 24) & 0x000000ff00000000 | (buf_addr[2] << 16) & 0x0000ff0000000000 |
        #     (buf_addr[1] << 8) & 0x00ff000000000000 | (buf_addr[0]) & 0xff00000000000000
        # )
        csi_status.csi_len = (buf_addr[9] << 8) & 0xff00 | (buf_addr[8] & 0x00ff)
        csi_status.channel = (buf_addr[11] << 8) & 0xff00 | (buf_addr[10] & 0x00ff)
        csi_status.buf_len = (buf_addr[cnt - 1] << 8) & 0xff00 | (buf_addr[cnt - 2] & 0x00ff)
        csi_status.payload_len = (
            (buf_addr[CSI_ST_LEN + 1] << 8) & 0xff00 |
            (buf_addr[CSI_ST_LEN]) & 0x00ff
        )

    csi_status.phyerr = buf_addr[12]
    csi_status.noise = buf_addr[13]
    csi_status.rate = buf_addr[14]
    csi_status.chanBW = buf_addr[15]
    csi_status.num_tones = buf_addr[16]
    csi_status.nr = buf_addr[17]
    csi_status.nc = buf_addr[18]
    csi_status.rssi = buf_addr[19]
    csi_status.rssi_0 = buf_addr[20]
    csi_status.rssi_1 = buf_addr[21]
    csi_status.rssi_2 = buf_addr[22]

    return csi_status


class COMPLEX:
    def __init__(self):
        self.real = 0
        self.imag = 0


def bit_convert(data, maxbit):
    if data & (1 << (maxbit - 1)):
        data -= (1 << maxbit)
    return data


def fill_csi_matrix(csi_addr, nr, nc, num_tones):

    # COMPLEX csi_matrix[3][3][114];
    csi_matrix = [[[COMPLEX() for k in range(num_tones)] for j in range(nc)] for i in range(nr)]

    bits_left = 16
    bitmask = (1 << 10) - 1
    idx = 0
    h_data = csi_addr[idx]
    idx += 1
    h_data += (csi_addr[idx] << 8)
    idx += 1
    current_data = h_data & ((1 << 16) - 1)

    for k in range(num_tones):
        for nc_idx in range(nc):
            for nr_idx in range(nr):
                if (bits_left - 10) < 0:
                    h_data = csi_addr[idx]
                    idx += 1
                    h_data += (csi_addr[idx] << 8)
                    idx += 1
                    current_data += h_data << bits_left
                    bits_left += 16

                imag = current_data & bitmask
                imag = bit_convert(imag, 10)
                csi_matrix[nr_idx][nc_idx][k].imag = imag

                bits_left -= 10
                current_data = current_data >> 10

                if (bits_left - 10) < 0:
                    h_data = csi_addr[idx]
                    idx += 1
                    h_data += (csi_addr[idx] << 8)
                    idx += 1
                    current_data += h_data << bits_left
                    bits_left += 16

                real = current_data & bitmask
                real = bit_convert(real, 10)
                csi_matrix[nr_idx][nc_idx][k].real = real

                bits_left -= 10
                current_data = current_data >> 10

    return csi_matrix


class CSIConverter:

    def __init__(self, filename):
        self.filename = filename

    def write(self, frame: bytes, length: int, timestamp: int):

        csi_status = record_status(frame, length)
        csi_matrix = fill_csi_matrix(frame[CSI_ST_LEN:], csi_status.nr, csi_status.nc, csi_status.num_tones)
        print(csi_status)

