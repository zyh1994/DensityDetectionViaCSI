import matplotlib.pyplot as plt
import numpy as np
import cv2


def align_matrix(csi_matrix, nrx = 3, ntx = 3, nsubcarrier = 56):
    pass


def gen_heat_map(data: list):

    # p

    # align the matrix to the same size
    for csi_data in data:
        csi_data = align_matrix(csi_data)