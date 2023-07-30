import matplotlib.pyplot as plt
import numpy as np

from common import load_csv_from_folder
from kalman import apply_kalman_filter
from utilities.CSVFrameInterpolation import data_interpolate


def using_kalman_filter(csi_db_matrix):
    # Clone the CSI matrix for filtered CSI matrix
    csi_matrix_filtered = np.copy(csi_db_matrix)

    # Using Kalman filter to filter the CSI matrix
    csi_matrix_filtered = apply_kalman_filter(csi_matrix_filtered)

    return csi_matrix_filtered


def main(folder_path: list):
    # Initialize the figure and the subplots
    fig, (ax1, ax2) = plt.subplots(2)

    # Make space for the Slider
    fig.subplots_adjust(bottom=0.2)

    # Load the CSI data from the csv file
    csi_info_list = []
    for path in folder_path:

        # Get the CSI data from the csv file
        csv_frames = load_csv_from_folder(path)

        # Padding the data to make sure the data is in the same size
        csv_frames = data_interpolate(csv_frames, 1500, 60)

        # Append the padded CSI data to the list
        csi_info_list.append(csv_frames)

        # Print out the size of csi info list
        print("The size of csi info list is: {}".format(len(csv_frames)))

    # # Initialize slider
    # axcolor = 'lightgoldenrodyellow'
    # axslider = plt.axes([0.25, 0.1, 0.65, 0.03], facecolor=axcolor)  # Slider position
    # slider = Slider(axslider, 'Frame', 0, len(csi_info_list) - 1, valinit=0, valstep=1)
    #
    # def update(val):
    #     # Clear the figure for the next plot
    #     ax1.cla()
    #     ax2.cla()
    #
    #     # Get the CSI data
    #     csi_info = csi_info_list[int(val)]
    #
    #     # Get the amplitude matrix
    #     csi_matrix = db_matrix(preprocess_matrix(csi_info.data()))
    #
    #     # Compress the CSI matrix to one dimension
    #     csi_matrix = csi_matrix.reshape(-1)
    #
    #     # Using Kalman filter to filter the CSI matrix
    #     csi_matrix_filtered = using_kalman_filter(csi_matrix)
    #
    #     # Draw the graphs
    #     ax1.plot(csi_matrix)
    #     ax1.set_title('csi_matrix')
    #
    #     ax2.plot(csi_matrix_filtered)
    #     ax2.set_title('csi_matrix_filtered')
    #
    #     # Draw the frame index on the left top corner
    #     fig.suptitle("Frame Index: {}".format(int(val)))
    #
    #     fig.canvas.draw_idle()
    #
    # slider.on_changed(update)
    #
    # # Call update once to draw the initial plot
    # update(0)
    #
    # plt.show()

    print("Done!")


if __name__ == '__main__':

    BASE_DIR = "/Users/orlando/OneDrive/data/OpenWRT_20230717_crossfire"

    main([
        BASE_DIR + "/crossfire_945/202307160030",
        BASE_DIR + "/crossfire_957/202307160031",
        BASE_DIR + "/solo_945/202307160048",
        BASE_DIR + "/solo_957/202307160041",
    ])
