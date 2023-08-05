import os
import sys

import cv2
import numpy as np

from csi.CSVDataLoader import load_csi_from_csv
from filter.KalmanFilter import kalman_filter


def load_csv_from_folder(path: str):
    # if the folder path is not valid, exit the program
    if not os.path.isdir(path):
        print("The folder path is not valid!")
        sys.exit(1)

    # load the csi data from the csv file
    csi_info_list = load_csi_from_csv(os.path.join(path, "csi_data.csv"))
    if csi_info_list is None or len(csi_info_list) == 0:
        print("The csi_info_list is empty!")
        sys.exit(1)

    return csi_info_list


def load_frames_from_folder(path: str):
    img_list = sorted(os.listdir(path))
    img_data_list = []

    for img_name in img_list:
        img_path = os.path.join(path, img_name)
        img_data = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
        img_data_list.append(img_data)

    return img_data_list


def apply_kalman_filter(raw_signal):
    # Define the parameters of Kalman filter
    initial_state_estimate = np.array([0.0])  # 初始状态估计值
    initial_estimate_covariance = np.eye(1)  # 初始状态估计的协方差矩阵
    process_noise_covariance = np.diag([0.01])  # 过程噪音协方差矩阵
    measurement_noise_covariance = 0.04  # 测量噪音的方差

    # Using Kalman filter to filter the raw signal
    filtered_signal = kalman_filter(raw_signal,
                                    initial_state_estimate,
                                    initial_estimate_covariance,
                                    process_noise_covariance,
                                    measurement_noise_covariance)

    return filtered_signal
