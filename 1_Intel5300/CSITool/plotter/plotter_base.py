import matplotlib.pyplot as plt
import numpy as np

from Bfee import Bfee
from CSIUtilities import get_scaled_csi


def parse_csi_data(data_len, raw_frame):
    # Read the first byte from the frame
    frame_code = raw_frame[0]

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


def prepare_chart(title):
    # Set up real-time plotting
    plt.ion()
    fig, ax = plt.subplots()

    # Change the figue title
    fig.canvas.manager.set_window_title(title)

    return ax


def update_chart(ax, meta_info, abs_csi):
    # Clear the previous plot
    ax.clear()

    # Plot the CSI matrix
    ax.plot(abs_csi)

    # Plot the meta information on the top left corner
    # The information we want to show is:
    # timestamp_low
    # Nrx
    # Ntx
    # rssi_a
    # rssi_b
    # rssi_c
    # noise
    # agc
    # antenna_sel
    # perm
    # Each line of the meta information is separated by a new line character
    titles = ["timestamp_low", "Nrx", "Ntx", "rssi_a", "rssi_b", "rssi_c", "noise", "agc", "antenna_sel", "perm"]
    items = [meta_info["timestamp_low"], meta_info["Nrx"], meta_info["Ntx"], meta_info["rssi_a"],
                meta_info["rssi_b"], meta_info["rssi_c"], meta_info["noise"], meta_info["agc"], 
                meta_info["antenna_sel"], meta_info["perm"]]

    # Convert the list to the string
    description = "\n".join([f"{title}: {item}" for title, item in zip(titles, items)])

    # Set the information
    ax.text(0.01, 0.99, description, transform=ax.transAxes, fontsize=10,
            verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.2))

    # Set the legend and the axis labels
    ax.set_xlabel('Subcarrier index')
    ax.set_ylabel('SNR [dB]')

    # Pause for 0.01 second
    plt.pause(0.01)