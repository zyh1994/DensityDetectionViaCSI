import numpy as np


def preprocess_original_matrix(csi_matrix):
    # combine the real and imaginary part of the CSI matrix to form a complex number
    csi_matrix = csi_matrix[:, :, :, 0] + 1j * csi_matrix[:, :, :, 1]

    # get the maximum value of the CSI matrix
    max_csi = np.max(csi_matrix)

    # get the minimum value of the CSI matrix
    min_csi = np.min(csi_matrix)

    # normalize the CSI matrix to [0, 1]
    csi_matrix = (csi_matrix - min_csi) / (max_csi - min_csi)

    return csi_matrix


def get_amplitude_matrix(csi_matrix):

    # get the number of receive antennas, transmit antennas and subcarriers
    nr, nc, tones = csi_matrix.shape

    # convert the shape of the CSI matrix to (channels, subcarriers)
    csi_matrix = np.reshape(csi_matrix, (nr * nc, tones))

    # get the amplitude of the CSI matrix
    csi_matrix = np.abs(csi_matrix)

    return csi_matrix


def get_db_matrix(csi_matrix):

    # get the number of receive antennas, transmit antennas and subcarriers
    nr, nc, tones = csi_matrix.shape

    # convert the shape of the CSI matrix to (channels, subcarriers)
    csi_matrix = np.reshape(csi_matrix, (nr * nc, tones))

    # get the amplitude of the CSI matrix
    csi_matrix = np.abs(csi_matrix)

    # convert the amplitude to dB
    csi_matrix = 20 * np.log10(csi_matrix)

    return csi_matrix


def get_phase_matrix(csi_matrix):

    # get the number of receive antennas, transmit antennas and subcarriers
    nr, nc, tones = csi_matrix.shape

    # convert the shape of the CSI matrix to (channels, subcarriers)
    csi_matrix = np.reshape(csi_matrix, (nr * nc, tones))

    # get the phase of the CSI matrix
    csi_matrix = np.angle(csi_matrix)

    return csi_matrix

