import os
import sys

import cv2
import numpy as np
from matplotlib import pyplot as plt

from common import load_frames_from_folder, apply_kalman_filter
from csi.CSVDataLoader import load_csi_from_csv
from utilities.MatrixPreprocessor import preprocess_matrix, db_matrix


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
    fig, (ax1, ax2) = plt.subplots(1, 2)

    # Set the figure size to 720p
    fig.set_size_inches(12.80, 7.20)

    # If the folder path is not valid, exit the program
    if not os.path.isdir(folder_path):
        print("The folder path is not valid!")
        sys.exit(1)

    # Load the csi data from the csv file
    csi_info_list = load_csi_from_csv(os.path.join(folder_path, "csi_data.csv"))
    if csi_info_list is None or len(csi_info_list) == 0:
        print("The csi_info_list is empty!")
        sys.exit(1)

    # Load images from the folder
    csi_frames_list = load_frames_from_folder(os.path.join(folder_path, "png"))
    if csi_frames_list is None or len(csi_frames_list) == 0:
        print("The csi_frames_list is empty!")
        sys.exit(1)

    # Check the length of the csi_info_list and the csi_frames_list
    if len(csi_info_list) != len(csi_frames_list):
        print("The length of the csi_info_list and the csi_frames_list is not equal!")
        sys.exit(1)

    # Show the CSI data and the image
    for idx in range(len(csi_info_list)):
        # Clear the figure for the next plot
        ax1.cla()
        ax2.cla()

        # Get the CSI data
        csi_info = csi_info_list[idx]

        # Get the amplitude matrix
        csi_matrix = db_matrix(preprocess_matrix(csi_info.data()))

        # Using Kalman filter to filter the CSI matrix
        csi_matrix_filtered = using_kalman_filter(csi_matrix)

        # Reshape the CSI matrix
        csi_matrix_filtered = csi_matrix_filtered.reshape(-1, csi_info.number_of_tones)  # (3, 3, 56) -> (9, 56)

        # The default of image is BGR, so we need to convert it to RGB
        csi_frame = cv2.cvtColor(csi_frames_list[idx], cv2.COLOR_BGR2RGB)

        # Resize the image to 720p
        csi_frame = cv2.resize(csi_frame, (1280, 720))

        # Plot the CSI matrix in the line chart, and the image on the left
        ax1.imshow(csi_frame)
        ax2.plot(csi_matrix_filtered.T)

        # Add the frame index to the title
        ax1.set_title("Frame Index: {}".format(idx))

        # Show the figure
        plt.pause(0.1)

    print("Done!")


if __name__ == '__main__':
    if len(sys.argv) > 1:
        main(sys.argv[1])
    else:
        print("Please input the folder path!")
