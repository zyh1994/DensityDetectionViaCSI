import os
import sys

import matplotlib.pyplot as plt

from csi.CSVDataLoader import load_csi_from_csv
from plot.HeattMapChart import gen_heatmap_matrix
from utilities.MatrixPreprocessor import padding_csi_matrix


def main(folder_path: str):
    # if the folder path is not valid, exit the program
    if not os.path.isdir(folder_path):
        print("The folder path is not valid!")
        sys.exit(1)

    # load the csi data from the csv file
    csi_info_list = load_csi_from_csv(os.path.join(folder_path, "csi_data.csv"))
    if csi_info_list is None or len(csi_info_list) == 0:
        print("The csi_info_list is empty!")
        sys.exit(1)

    # padding the csi matrix
    csi_info_list = padding_csi_matrix(csi_info_list)

    # get the heat map of the CSI data
    heat_map = gen_heatmap_matrix(csi_info_list)

    # plot the heat map
    fig, ax = plt.subplots(figsize=(10, 8))
    cax = ax.imshow(heat_map, cmap='viridis', interpolation='nearest')

    # add colorbar
    cbar = fig.colorbar(cax)

    # set colorbar label
    cbar.set_label('CSI Magnitude (dB)')

    # add grid lines
    ax.grid(color='white', linestyle='-', linewidth=0.5)

    # set x and y axis labels
    ax.set_xlabel('Time')
    ax.set_ylabel('Subcarrier')

    # set title
    ax.set_title('CSI Heatmap')

    # show the heat map
    plt.show()


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python3 hotmap.py <folder_path>")
        sys.exit(1)

    main(sys.argv[1])
