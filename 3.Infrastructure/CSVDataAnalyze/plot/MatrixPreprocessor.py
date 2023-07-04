import numpy as np


def preprocess_matrix(csi_matrix):
    # combine the real and imaginary part of the CSI matrix to form a complex number
    csi_matrix = csi_matrix[:, :, :, 0] + 1j * csi_matrix[:, :, :, 1]

    # get the maximum value of the CSI matrix
    max_csi = np.max(csi_matrix) + 1e-10

    # get the minimum value of the CSI matrix
    min_csi = np.min(csi_matrix)

    # normalize the CSI matrix to [0, 1]
    csi_matrix = (csi_matrix - min_csi) / (max_csi - min_csi)

    return csi_matrix


def amplitude_matrix(csi_matrix):

    # get the amplitude of the CSI matrix
    csi_matrix = np.abs(csi_matrix)

    return csi_matrix


def db_matrix(csi_matrix):
    # get the amplitude of the CSI matrix
    csi_matrix_abs = np.abs(csi_matrix)

    # convert the amplitude to dB
    csi_matrix_db = 20 * np.log10(csi_matrix_abs)

    return csi_matrix_db


def phase_matrix(csi_matrix):

    # get the phase of the CSI matrix
    csi_matrix = np.angle(csi_matrix)

    return csi_matrix

