import numpy as np


class Bfee(object):
    """
    struct iwl_bfee_notif
    {
        uint32_t / * __le32 * / timestamp_low;
        uint16_t / * __le16 * / bfee_count;
        uint16_t reserved1;
        uint8_t Nrx, Ntx;
        int8_t rssiA, rssiB, rssiC;
        int8_t noise;
        uint8_t agc, antenna_sel;
        uint16_t / * __le16 * / len;
        uint16_t / * __le16 * / fake_rate_n_flags;
        uint8_t payload[0];
    } __attribute__((packed));
    """

    def __init__(self, bytes) -> None:
        # Convert the bytes to a numpy array
        bytes = np.frombuffer(bytes, dtype=np.uint8)

        # Four bytes for timestamp
        # A timestamp that indicates the time at which the CSI was measured.
        self.timestamp_low = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24)

        # Two bytes for bfee_count
        # The number of the current CSI measurement in the current data file.
        self.bfee_count = bytes[4] + (bytes[5] << 8)

        # Two bytes for reserved1
        # Not used
        self.reserved_not_used = bytes[6] + (bytes[7] << 8)

        # One byte for Nrx
        # The number of receiving antennas.
        self.nRx = bytes[8]

        # One byte for Ntx
        # The number of transmitting antennas.
        self.nTx = bytes[9]

        # One byte for rssi_a
        # rssi_a - the Received Signal Strength Indicator (RSSI) of the wireless signal measured by the antenna.
        self.rssi_a = bytes[10]

        # One byte for rssi_b
        # rssi_b - the Received Signal Strength Indicator (RSSI) of the wireless signal measured by the antenna.
        self.rssi_b = bytes[11]

        # One byte for rssi_c
        # rssi_c - the Received Signal Strength Indicator (RSSI) of the wireless signal measured by the antenna.
        self.rssi_c = bytes[12]

        # One byte for noise
        # The noise level of the wireless signal.
        self.noise = bytes[13] - 256

        # One byte for agc
        # Automatic Gain Control (AGC) value used to amplify the received signal.
        self.agc = bytes[14]

        # One byte for antenna_sel
        # A binary value indicating which receiving antennas were selected to measure the CSI.
        self.antenna_sel = bytes[15]

        # We can also derive the antenna configuration from the antenna_sel field
        # A permutation matrix used to reorder the CSI matrix based on the selected receiving antennas.
        self.perm = np.zeros(3)
        self.perm[0] = (self.antenna_sel & 0x3) + 1
        self.perm[1] = ((self.antenna_sel >> 2) & 0x3) + 1
        self.perm[2] = ((self.antenna_sel >> 4) & 0x3) + 1

        # Two bytes for len
        # The length of the payload that contains the CSI information.
        self.len = bytes[16] + (bytes[17] << 8)

        # Two bytes for fake_rate_n_flags
        # A field that contains various flags and information about the CSI.
        self.fake_rate_n_flags = bytes[18] + (bytes[19] << 8)

        # Remaining bytes for payload, starting at byte 21 to the end
        # The actual CSI matrix that describes the wireless channel state between the transmitting and receiving
        # antennas. The matrix is a 3D complex matrix of size 30xNrxxNtx, where 30 is the number of subcarriers and
        # Nrx and Ntx are the number of receiving and transmitting antennas, respectively.
        # self._calc_len = (30 * (self.Nrx * self.Ntx * 8 * 2 + 3) + 6) / 8
        # self.csi = self._load_payload(bytes[20:])
        self._payload = bytes[20:]

    def get_meta_info(self):
        meta_dict = {
            'timestamp_low': self.timestamp_low,
            'bfee_count': self.bfee_count,
            'reserved1': self.reserved_not_used,
            'Nrx': self.nRx,
            'Ntx': self.nTx,
            'rssi_a': self.rssi_a,
            'rssi_b': self.rssi_b,
            'rssi_c': self.rssi_c,
            'noise': self.noise,
            'agc': self.agc,
            'antenna_sel': self.antenna_sel,
            'perm': self.perm,
            'len': self.len,
            'fake_rate_n_flags': self.fake_rate_n_flags
        }

        return meta_dict

    def get_payload_matrix(self):
        """
        subcarrier structure:

        length        | type    | name      | description
        --------------|---------|-----------|------------
        3             | u8      | csi       | subcarrier header
        8 * Nrx * Ntx | complex | csi_entry | CSI matrix entry

        
        Complex number structure:

        length | type | name | description
        -------|------|------|------------
        8      | u8   | real | real part of the complex number
        8      | u8   | imag | imaginary part of the complex number
        """

        # Create a numpy array for the CSI matrix
        csi_matrix = np.zeros((self.nRx, self.nTx, 30), dtype=np.complex64)

        # Index of the current byte in the payload
        idx = 0

        # Iterate over the subcarriers (30 subcarriers)
        for carrier in range(30):

            # Skip the first 3 bytes, because they are header information
            idx += 3
            remainder = idx % 8

            for ntx in range(self.nTx):
                for nrx in range(self.nRx):

                    # Extract the real and imaginary part of the complex number
                    real_bin = bytes([(self._payload[idx // 8] >> remainder) |
                                      (self._payload[idx // 8 + 1] << (8 - remainder)) & 0b11111111])
                    real = int.from_bytes(real_bin, byteorder='little', signed=True)
                    imag_bin = bytes([(self._payload[idx // 8 + 1] >> remainder) |
                                      (self._payload[idx // 8 + 2] << (8 - remainder)) & 0b11111111])
                    imag = int.from_bytes(imag_bin, byteorder='little', signed=True)

                    # Create the complex number and store it in the CSI matrix
                    csi_matrix[nrx, ntx, carrier] = complex(float(real), float(imag))

                    # Increment the index by 16 bits (real and imaginary part)
                    idx += 16

        # return the CSI matrix
        return csi_matrix
