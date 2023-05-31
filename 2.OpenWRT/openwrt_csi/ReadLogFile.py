import struct

from ReadCSI import read_csi


def read_log_file(filename):
    with open(filename, 'rb') as f:
        data = f.read()

    endian_format = '>'
    ret = []
    cur = 0
    count = 0
    data_len = len(data)

    while cur < (data_len - 420):
        field_len, timestamp, csi_len, tx_channel, err_info, noise_floor, Rate, bandWidth, num_tones, nr, nc, rssi, rssi1, rssi2, rssi3, payload_len = struct.unpack(
            endian_format + 'HQBH2b5H')
        cur += 420

        if cur + field_len > data_len:
            break

        csi_matrix = {
            'timestamp': timestamp,
            'csi_len': csi_len,
            'channel': tx_channel,
            'err_info': err_info,
            'noise_floor': noise_floor,
            'Rate': Rate,
            'bandWidth': bandWidth,
            'num_tones': num_tones,
            'nr': nr,
            'nc': nc,
            'rssi': rssi,
            'rssi1': rssi1,
            'rssi2': rssi2,
            'rssi3': rssi3,
            'payload_len': payload_len
        }

        if csi_len > 0:
            csi_buf = data[cur:cur + csi_len]
            csi = read_csi(csi_buf, nr, nc, num_tones)
            cur += csi_len
            csi_matrix['csi'] = csi
        else:
            csi_matrix['csi'] = []

        if payload_len > 0:
            data_buf = data[cur:cur + payload_len]
            cur += payload_len
            csi_matrix['payload'] = data_buf
        else:
            csi_matrix['payload'] = []

        count += 1
        ret.append(csi_matrix)

    return ret

def main():
    filename = 'log_file.bin'
    result = read_log_file(filename)
    print(result)

if __name__ == "__main__":
    main()
