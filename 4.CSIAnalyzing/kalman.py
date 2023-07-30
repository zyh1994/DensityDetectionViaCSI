import os
import sys

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.widgets import Slider

from csi.CSVDataLoader import load_csi_from_csv
from filter.KalmanFilter import kalman_filter
from utilities.MatrixPreprocessor import preprocess_matrix, db_matrix


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


def using_kalman_filter(csi_db_matrix):
    # Get the shape of the CSI matrix
    nrx, ntx, _ = csi_db_matrix.shape

    # Clone the CSI matrix for filtered CSI matrix
    csi_db_matrix_filtered = np.copy(csi_db_matrix)

    # Using Kalman filter to filter the CSI matrix
    for i in range(nrx):
        for j in range(ntx):
            csi_db_matrix_filtered[i][j] = apply_kalman_filter(csi_db_matrix[i][j])

    return csi_db_matrix_filtered


def main(folder_path: str):
    # Initialize the figure and the subplots
    fig, (ax1, ax2) = plt.subplots(2)

    fig.subplots_adjust(bottom=0.2)  # Make space for the Slider

    # if the folder path is not valid, exit the program
    if not os.path.isdir(folder_path):
        print("The folder path is not valid!")
        sys.exit(1)

    # load the csi data from the csv file
    csi_info_list = load_csi_from_csv(os.path.join(folder_path, "csi_data.csv"))
    if csi_info_list is None or len(csi_info_list) == 0:
        print("The csi_info_list is empty!")
        sys.exit(1)

    # Initialize slider
    axcolor = 'lightgoldenrodyellow'
    axslider = plt.axes([0.25, 0.1, 0.65, 0.03], facecolor=axcolor)  # Slider position
    slider = Slider(axslider, 'Frame', 0, len(csi_info_list) - 1, valinit=0, valstep=1)

    def update(val):
        # Clear the figure for the next plot
        ax1.cla()
        ax2.cla()

        # Get the CSI data
        csi_info = csi_info_list[int(val)]

        # Get the amplitude matrix
        csi_matrix = db_matrix(preprocess_matrix(csi_info.data()))

        # Using Kalman filter to filter the CSI matrix
        csi_matrix_filtered = using_kalman_filter(csi_matrix)

        # Reshape the CSI matrix
        csi_matrix_filtered = csi_matrix_filtered.reshape(-1, csi_info.number_of_tones)  # (3, 3, 56) -> (9, 56)
        csi_matrix = csi_matrix.reshape(-1, csi_info.number_of_tones)

        # Draw the graphs
        ax1.plot(csi_matrix.transpose())
        ax1.set_title('csi_matrix')

        ax2.plot(csi_matrix_filtered.transpose())
        ax2.set_title('csi_matrix_filtered')

        # Draw the frame index on the left top corner
        fig.suptitle("Frame Index: {}".format(int(val)))

        fig.canvas.draw_idle()

    slider.on_changed(update)

    # Call update once to draw the initial plot
    update(0)

    plt.show()

    print("Done!")


if __name__ == '__main__':
    if len(sys.argv) > 1:
        main(sys.argv[1])
    else:
        print("Please input the folder path!")
