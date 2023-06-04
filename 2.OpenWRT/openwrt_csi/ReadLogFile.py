import numpy as np
import struct
import os

def read_log_file(filename):
    try:
        with open(filename, 'rb') as f:
            len = os.path.getsize(filename)
            print('file length is: {}'.format(len))
            
            ret = [None] * int(np.ceil(len / 420))
            cur = 0
            count = 0

            while cur < (len - 4):
                field_len = struct.unpack(">H", f.read(2))[0]
                cur += 2
                print('Block length is: {}'.format(field_len))

                print(field_len, cur, len)

                if (cur + field_len) > len:
                    break
                
                timestamp = struct.unpack(">Q", f.read(8))[0]
                csi_matrix = {"timestamp": timestamp}
                cur += 8
                print('timestamp is {}'.format(timestamp))

                csi_len = struct.unpack(">H", f.read(2))[0]
                csi_matrix["csi_len"] = csi_len
                cur += 2
                print('csi_len is {}'.format(csi_len))

                tx_channel = struct.unpack(">H", f.read(2))[0]
                csi_matrix["channel"] = tx_channel
                cur += 2
                print('channel is {}'.format(tx_channel))

                # ... repeat for other fields ...

                # if csi_len > 0:
                #     csi_buf = struct.unpack("{}B".format(csi_len), f.read(csi_len))
                #     csi = read_csi(csi_buf, nr, nc, num_tones)
                #     cur += csi_len
                #     csi_matrix["csi"] = csi
                # else:
                #     csi_matrix["csi"] = 0
                
                # ... repeat for other fields ...

                if (cur + 420 > len):
                    break
                ret[count] = csi_matrix
                count += 1
            if (count >1):
                ret = ret[:count-1]
            else:
                ret = ret[0]
            return ret
    except IOError:
        print("couldn't open file {}".format(filename))


def main():
    sample_data = read_log_file('sample.dat')
    print(sample_data)


if __name__ == '__main__':
    main()
