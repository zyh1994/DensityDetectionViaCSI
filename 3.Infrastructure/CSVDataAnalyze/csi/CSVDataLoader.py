import csv
import json


from csi.BaseData import CSIBaseData


class CSIDataFromCSV(CSIBaseData):

    """
    timestamp: 时间戳
    csi_length: CSI的长度
    channel_number: 信道编号
    buffer_length: 缓冲区长度
    payload_length: 数据载荷长度
    physical_error: 物理错误
    noise_level: 噪声水平
    transmission_rate: 传输速率
    channel_bandwidth: 信道带宽
    number_of_tones: 载波数量
    receiver_antennas: 接收天线数量
    transmitter_antennas: 发射天线数量
    received_signal_strength: 接收信号强度
    rssi_antenna_0: 第一个天线的RSSI（接收信号强度指示）
    rssi_antenna_1: 第二个天线的RSSI
    rssi_antenna_2: 第三个天线的RSSI
    csi_matrix: CSI矩阵
    """

    def __init__(self, row: str):

        super().__init__()

        # get the CSI status
        self.timestamp = int(row[0])
        self.csi_length = int(row[1])
        self.channel_number = int(row[2])
        self.buffer_length = int(row[3])
        self.payload_length = int(row[4])
        self.physical_error = int(row[5])
        self.noise_level = int(row[6])
        self.transmission_rate = int(row[7])
        self.channel_bandwidth = int(row[8])
        self.number_of_tones = int(row[9])
        self.receiver_antennas = int(row[10])
        self.transmitter_antennas = int(row[11])
        self.received_signal_strength = int(row[12])
        self.rssi_antenna_0 = int(row[13])
        self.rssi_antenna_1 = int(row[14])
        self.rssi_antenna_2 = int(row[15])
        self.csi_matrix = json.loads(row[16])

    
    def __str__(self):
        return "timestamp {}, csi_length {}, channel_number {}, buffer_length {}, payload_length {}, " \
                "physical_error {}, noise_level {}, transmission_rate {}, channel_bandwidth {}, " \
                "number_of_tones {}, receiver_antennas {}, transmitter_antennas {}, received_signal_strength {}, " \
                "rssi_antenna_0 {}, rssi_antenna_1 {}, rssi_antenna_2 {}, csi_matrix {}".format(
            self.timestamp, self.csi_length, self.channel_number, self.buffer_length, self.payload_length,
            self.physical_error, self.noise_level, self.transmission_rate, self.channel_bandwidth,
            self.number_of_tones, self.receiver_antennas, self.transmitter_antennas,
            self.received_signal_strength, self.rssi_antenna_0, self.rssi_antenna_1,
            self.rssi_antenna_2, self.csi_matrix)


def load_csi_from_csv(csi_file: str):

    csi_data = []

    with open(csi_file, "r") as f:
        reader = csv.reader(f, delimiter="\t")

        # skip the first line
        next(reader)

        for row in reader:
            csi_data.append(CSIDataFromCSV(row))

    return csi_data
