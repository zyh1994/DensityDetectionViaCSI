import numpy as np
import matplotlib.pyplot as plt
import struct
import time
import sys

from Bfee import Bfee
from CSIUtilities import get_scaled_csi


def parse_csi_data(data_len, raw_frame):
    # Read the first byte from the frame
    frame_code = raw_frame[0]

    # Compute the checksum of the frame
    # checksum = hashlib.md5(raw_frame).hexdigest()

    # Convert the bytes from 1 to the last one to the numpy array
    np_frame = np.frombuffer(raw_frame[1:], dtype=np.uint8)

    # Now trying to decode the csi frame
    if frame_code == 187:
        bfee = Bfee(np_frame)
        return bfee.get_meta_info(), get_scaled_csi(bfee)
    else:
        # If the frame code is not 187, then print out the error message
        print("The frame code is not 187, convert to CSI entry failed")
        return None, None


def update_plot(ax, abs_csi):
    ax.clear()
    ax.plot(abs_csi)
    ax.legend(['RX Antenna A', 'RX Antenna B', 'RX Antenna C'], loc='lower right')
    ax.set_xlabel('Subcarrier index')
    ax.set_ylabel('SNR [dB]')
    plt.pause(0.01)


def plot_csi(filename):
    # Open the local csi file
    csi_fd = open(filename, "rb")

    # Set up real-time plotting
    plt.ion()
    fig, ax = plt.subplots()

    # Change the figue title
    # fig.canvas.set_window_title('CSI Plotter')
    fig.canvas.manager.set_window_title(filename)

    for i in range(1000):
        # Read the length of bytes in the data
        loaded_bytes = csi_fd.read(2)
        if not loaded_bytes:
            break

        # Convert the data to the short type
        data_len = struct.unpack('>H', loaded_bytes)[0]

        # Load the reset bytes of the data frame
        loaded_bytes = csi_fd.read(data_len)

        # Processing the data and plot it to the chart
        meta, csi = parse_csi_data(data_len, loaded_bytes)
        if meta is not None and csi is not None:
            # Squeeze and take the absolute value
            abs_csi = np.abs(np.squeeze(csi))

            # Transpose to match the Matlab example
            abs_csi = abs_csi.T

            # If the size of abs_csi is above 2 dimensions, then take the mean
            if abs_csi.ndim > 2:
                abs_csi = np.mean(abs_csi, axis=2)

            print(abs_csi.shape)

            # Update the plot
            update_plot(ax, abs_csi)

        # Sleep for 0.2 seconds
        time.sleep(0.5)

    # Close the csi file
    csi_fd.close()

if __name__ == "__main__":

    # Read the user input
    if len(sys.argv) > 1:
        filename = sys.argv[1]
    else:
        filename = "csi.dat"

    # Plot the CSI data
    plot_csi(filename)