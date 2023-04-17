import struct
import sys
import time

from plotter_base import *


def main(filename):
    # Open the local csi file
    csi_fd = open(filename, "rb")

    # Set up real-time plotting
    ax = prepare_chart(f"Offline: {filename}")

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
            # Compute the absolute value of the CSI matrix
            abs_csi = np.abs(csi)

            # Convert the shape of the CSI matrix to the 2D array
            # From (nTx, nRx, nSC) to (nAnt, nSC)
            abs_csi = np.reshape(abs_csi, (abs_csi.shape[0] * abs_csi.shape[1], abs_csi.shape[2]))

            # Transpose the CSI matrix and plot it
            update_chart(ax, meta, abs_csi.T)

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
    main(filename)
