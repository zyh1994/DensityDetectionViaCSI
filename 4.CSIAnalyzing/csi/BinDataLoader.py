from csi.BaseData import CSIBaseData


class CSIDataFromBin(CSIBaseData):

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

    def __init__(self, raw: bytes):
        super().__init__()

        # get the CSI status
        self.timestamp = None
        self.csi_length = None
        self.channel_number = None
        self.buffer_length = None
        self.payload_length = None
        self.physical_error = None
        self.noise_level = None
        self.transmission_rate = None
        self.channel_bandwidth = None
        self.number_of_tones = None
        self.receiver_antennas = None
        self.transmitter_antennas = None
        self.received_signal_strength = None
        self.rssi_antenna_0 = None
        self.rssi_antenna_1 = None
        self.rssi_antenna_2 = None
        self.csi_matrix = None