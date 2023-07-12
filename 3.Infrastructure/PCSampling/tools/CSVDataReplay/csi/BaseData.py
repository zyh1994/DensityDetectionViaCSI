import numpy as np
from collections import namedtuple


class CSIBaseData:

    def __init__(self):
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

    def meta_dict(self):
        return {
            "timestamp": self.timestamp,
            "csi_length": self.csi_length,
            "channel_number": self.channel_number,
            "buffer_length": self.buffer_length,
            "payload_length": self.payload_length,
            "physical_error": self.physical_error,
            "noise_level": self.noise_level,
            "transmission_rate": self.transmission_rate,
            "channel_bandwidth": self.channel_bandwidth,
            "number_of_tones": self.number_of_tones,
            "receiver_antennas": self.receiver_antennas,
            "transmitter_antennas": self.transmitter_antennas,
            "received_signal_strength": self.received_signal_strength,
            "rssi_antenna_0": self.rssi_antenna_0,
            "rssi_antenna_1": self.rssi_antenna_1,
            "rssi_antenna_2": self.rssi_antenna_2,
        }
    
    def meta(self):
        # convert the dictionary to an object
        return namedtuple("CSI_META", self.meta_dict().keys())(**self.meta_dict())
    
    def data(self):
        if self.csi_matrix is None:
            raise ValueError("csi_matrix is None")
        else:
            return np.array(self.csi_matrix)