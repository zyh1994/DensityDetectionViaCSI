import csv
import json

CSI_ST_LEN = 23


def bit_convert(data, maxbit):
    if data & (1 << (maxbit - 1)):
        data -= (1 << maxbit)
    return data


class COMPLEX:
    def __init__(self):
        self.real = 0
        self.imag = 0

    def __str__(self):
        if self.imag >= 0:
            return "{}+{}j".format(self.real, self.imag)
        else:
            return "{}{}j".format(self.real, self.imag)


class CSIStatus:

    def __init__(self):
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

    def parse(self, frame, frame_size, timestamp):
        self.timestamp = timestamp
        self.csi_length = (frame[8] << 8) & 0xff00 | (frame[9] & 0x00ff)
        self.channel_number = (frame[10] << 8) & 0xff00 | (frame[11] & 0x00ff)
        self.buffer_length = (frame[frame_size - 2] << 8) & 0xff00 | (frame[frame_size - 1] & 0x00ff)
        self.payload_length = ((frame[CSI_ST_LEN] << 8) & 0xff00 | (frame[CSI_ST_LEN + 1]) & 0x00ff)

        self.physical_error = frame[12]
        self.noise_level = frame[13]
        self.transmission_rate = frame[14]
        self.channel_bandwidth = frame[15]
        self.number_of_tones = frame[16]
        self.receiver_antennas = frame[17]
        self.transmitter_antennas = frame[18]
        self.received_signal_strength = frame[19]
        self.rssi_antenna_0 = frame[20]
        self.rssi_antenna_1 = frame[21]
        self.rssi_antenna_2 = frame[22]

        self.csi_matrix = self.fill_csi_matrix(frame[CSI_ST_LEN + 2:])

    def fill_csi_matrix(self, frame):

        # COMPLEX csi_matrix[3][3][114];
        csi_matrix = [[[COMPLEX() for k in range(self.number_of_tones)] for j in range(self.transmitter_antennas)] for i
                      in range(self.receiver_antennas)]

        bits_left = 16
        bitmask = (1 << 10) - 1
        idx = 0
        h_data = frame[idx]
        idx += 1
        h_data += (frame[idx] << 8)
        idx += 1
        current_data = h_data & ((1 << 16) - 1)

        for k in range(self.number_of_tones):
            for nc_idx in range(self.transmitter_antennas):
                for nr_idx in range(self.receiver_antennas):
                    if (bits_left - 10) < 0:
                        h_data = frame[idx]
                        idx += 1
                        h_data += (frame[idx] << 8)
                        idx += 1
                        current_data += h_data << bits_left
                        bits_left += 16

                    imag = current_data & bitmask
                    imag = bit_convert(imag, 10)
                    csi_matrix[nr_idx][nc_idx][k].imag = imag

                    bits_left -= 10
                    current_data = current_data >> 10

                    if (bits_left - 10) < 0:
                        h_data = frame[idx]
                        idx += 1
                        h_data += (frame[idx] << 8)
                        idx += 1
                        current_data += h_data << bits_left
                        bits_left += 16

                    real = current_data & bitmask
                    real = bit_convert(real, 10)
                    csi_matrix[nr_idx][nc_idx][k].real = real

                    bits_left -= 10
                    current_data = current_data >> 10

        return csi_matrix

    def get_matrix(self):

        matrix = []
        for nr_idx in range(self.receiver_antennas):
            antennas_data = []
            for nc_idx in range(self.transmitter_antennas):
                tones_data = []
                for k in range(self.number_of_tones):
                    complex_data = (
                        self.csi_matrix[nr_idx][nc_idx][k].real,
                        self.csi_matrix[nr_idx][nc_idx][k].imag
                        )
                    tones_data.append(complex_data)
                antennas_data.append(tones_data)
            matrix.append(antennas_data)

        # Convert the matrix to a json string
        return json.dumps(matrix)


class CSIConverter:

    def __init__(self, filename):
        self.filename = filename

        # prepare csv file header, and use , as delimiter
        with open(self.filename, 'w', newline='') as csvfile:

            # Get csv writer
            writer = csv.writer(csvfile, delimiter='\t')

            # write headers
            writer.writerow(['timestamp', 'csi_length', 'channel_number', 'buffer_length', 'payload_length',
                             "physical_error", "noise_level", "transmission_rate", "channel_bandwidth",
                             "number_of_tones", "receiver_antennas", "transmitter_antennas",
                             "received_signal_strength", "rssi_antenna_0", "rssi_antenna_1",
                             "rssi_antenna_2", "csi_matrix"])

    def write(self, frame: bytes, length: int, timestamp: int):
        csi = CSIStatus()

        # Parse CSI status
        csi.parse(frame, length, timestamp)

        # Append to csv file
        with open(self.filename, 'a', newline='') as csvfile:

            # Get csv writer
            writer = csv.writer(csvfile, delimiter='\t')

            # Write row
            writer.writerow([csi.timestamp, csi.csi_length, csi.channel_number, csi.buffer_length, csi.payload_length,
                             csi.physical_error, csi.noise_level, csi.transmission_rate, csi.channel_bandwidth,
                             csi.number_of_tones, csi.receiver_antennas, csi.transmitter_antennas,
                             csi.received_signal_strength, csi.rssi_antenna_0, csi.rssi_antenna_1,
                             csi.rssi_antenna_2, csi.get_matrix()])
