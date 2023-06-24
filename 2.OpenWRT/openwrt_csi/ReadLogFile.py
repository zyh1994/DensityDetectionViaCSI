import numpy as np
import struct
import os

from ReadCSI import read_csi

def read_log_file(filename):
    # 打开指定文件名的文件，'rb'表示以二进制形式读取
    f = open(filename, 'rb')

    # 如果文件打开失败，则输出错误信息并返回
    if not f:
        raise Exception("couldn't open file {}".format(filename))

    # 将文件指针移动到文件的末尾，获取文件的长度
    f.seek(0, 2)
    len = f.tell()
    print('file length is: {}'.format(len))

    # 将文件指针移动回文件的开头，开始读取文件
    f.seek(0, 0)

    # 初始化输出结果的列表，预计每420字节为一个元素
    ret = []
    cur = 0  # 当前读取的位置
    count = 0  # 已读取的数据块的数量

    # 设置数据的字节序格式为大端
    endian_format = '>'

    # 循环读取文件，直到文件的末尾
    while cur < (len - 4):
        # 读取当前块的长度
        field_len = struct.unpack(endian_format + 'H', f.read(2))[0]
        cur += 2
        print('Block length is: {}'.format(field_len))

        # 如果当前块的长度大于剩余的文件长度，则跳出循环
        if (cur + field_len) > len:
            break

        # 读取并存储时间戳等各种参数，这些参数被存储在csi_matrix中
        # 每读取一个参数，cur都会增加对应的长度
        # 在读取每个参数后，都会打印出该参数的值
        csi_matrix = {}

        # 1. 时间戳
        timestamp = struct.unpack(endian_format + 'Q', f.read(8))[0]
        csi_matrix['timestamp'] = timestamp
        cur += 8
        print('timestamp is {}'.format(timestamp))

        # 2. CSI长度
        csi_len = struct.unpack(endian_format + 'H', f.read(2))[0]
        csi_matrix['csi_len'] = csi_len
        cur += 2
        print('csi_len is {}'.format(csi_len))

        # 3. 传输通道
        tx_channel = struct.unpack(endian_format + 'H', f.read(2))[0]
        csi_matrix['channel'] = tx_channel
        cur += 2
        print('channel is {}'.format(tx_channel))

        # 4. 错误信息
        err_info = struct.unpack(endian_format + 'B', f.read(1))[0]
        csi_matrix['err_info'] = err_info
        cur += 1
        print('err_info is {}'.format(err_info))

        # 5. 噪声水平
        noise_floor = struct.unpack(endian_format + 'B', f.read(1))[0]
        csi_matrix['noise_floor'] = noise_floor
        cur += 1
        print('noise_floor is {}'.format(noise_floor))

        # 6. 速率
        Rate = struct.unpack(endian_format + 'B', f.read(1))[0]
        csi_matrix['Rate'] = Rate
        cur += 1
        print('rate is {}'.format(Rate))

        # 7. 带宽
        bandWidth = struct.unpack(endian_format + 'B', f.read(1))[0]
        csi_matrix['bandWidth'] = bandWidth
        cur += 1
        print('bandWidth is {}'.format(bandWidth))

        # 8. 音调数量
        num_tones = struct.unpack(endian_format + 'B', f.read(1))[0]
        csi_matrix['num_tones'] = num_tones
        cur += 1
        print('num_tones is {}'.format(num_tones))

        # 9. 发送天线数量
        nr = struct.unpack(endian_format + 'B', f.read(1))[0]
        csi_matrix['nr'] = nr
        cur += 1
        print('nr is {}'.format(nr))

        # 10. 接收天线数量
        nc = struct.unpack(endian_format + 'B', f.read(1))[0]
        csi_matrix['nc'] = nc
        cur += 1
        print('nc is {}'.format(nc))

        # 11. RSSI
        rssi = struct.unpack(endian_format + 'B', f.read(1))[0]
        csi_matrix['rssi'] = rssi
        cur += 1
        print('rssi is {}'.format(rssi))

        # 12. RSSI1
        rssi1 = struct.unpack(endian_format + 'B', f.read(1))[0]
        csi_matrix['rssi1'] = rssi1
        cur += 1
        print('rssi1 is {}'.format(rssi1))

        # 13. RSSI2
        rssi2 = struct.unpack(endian_format + 'B', f.read(1))[0]
        csi_matrix['rssi2'] = rssi2
        cur += 1
        print('rssi2 is {}'.format(rssi2))

        # 14. RSSI3
        rssi3 = struct.unpack(endian_format + 'B', f.read(1))[0]
        csi_matrix['rssi3'] = rssi3
        cur += 1
        print('rssi3 is {}'.format(rssi3))

        # 15. 有效载荷长度
        payload_len = struct.unpack(endian_format + 'H', f.read(2))[0]
        csi_matrix['payload_len'] = payload_len
        cur += 2
        print('payload length: {}'.format(payload_len))

        # 如果csi_len > 0，读取CSI数据并将其添加到csi_matrix中
        if csi_len > 0:
            csi_buf = struct.unpack('{}B'.format(csi_len), f.read(csi_len))
            csi = read_csi(csi_buf, nr, nc, num_tones)
            cur += csi_len
            csi_matrix['csi'] = csi
        else:
            csi_matrix['csi'] = 0

        # 如果payload_len > 0，读取有效载荷并将其添加到csi_matrix中
        if payload_len > 0:
            data_buf = struct.unpack('{}B'.format(payload_len), f.read(payload_len))
            cur += payload_len
            csi_matrix['payload'] = data_buf
        else:
            csi_matrix['payload'] = 0

        # 每读取完一个数据块，都将csi_matrix添加到结果列表中，并增加计数器
        if (cur + 420 > len):
            break
        count += 1
        ret.append(csi_matrix)

    # 如果读取了多于一个数据块，则输出所有读取的数据块
    # 如果只读取了一个数据块，则输出该数据块
    if count > 1:
        ret = ret[:count-1]
    else:
        ret = ret[0] if ret else None

    # 关闭文件
    f.close()
    return ret



def main():
    sample_data = read_log_file('csi.dat')
    print(sample_data)


if __name__ == '__main__':
    main()
