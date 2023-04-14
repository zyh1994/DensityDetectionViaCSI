import numpy as np
import matplotlib.pyplot as plt
import socket
import struct

from Bfee import Bfee
from CSIUtilities import get_scaled_csi


def setup_client_side():
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect the socket to the port where the server is listening
    server_address = ('localhost', 8090)

    # Connect to the server
    sock.connect(server_address)

    return sock


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


def update_plot(ax, meta_info, abs_csi):
    
    # Clear the previous plot
    ax.clear()

    # Plot the CSI matrix
    ax.plot(abs_csi)

    # Plot the meta information on the top left corner
    ax.text(0.01, 0.99, meta_info, transform=ax.transAxes, fontsize=10,
            verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

    # Set the legend and the axis labels
    ax.set_xlabel('Subcarrier index')
    ax.set_ylabel('SNR [dB]')

    # Pause for 0.01 second
    plt.pause(0.01)


def main():

    # Set up real-time plotting
    plt.ion()
    fig, ax = plt.subplots()

    # Change the figue title
    fig.canvas.manager.set_window_title("CSI Plotter")

    # Get the client socket
    cli_sock = setup_client_side()

    while True:

        # If the socket is closed, then reconnect to the server
        if cli_sock._closed:
            cli_sock = setup_client_side()

        # Two bytes for the length of the data
        loaded_bytes = cli_sock.recv(2)
        if not loaded_bytes:
            print("No data received")
            continue
    
        # Convert the data to the short type
        data_len = struct.unpack('>H', loaded_bytes)[0]

        # Read the rest of the data
        raw_frame = cli_sock.recv(data_len)

        # Process the raw frame and convert it to the CSI entry
        meta_info, csi_mat = parse_csi_data(data_len, raw_frame)

        if meta_info is not None and csi_mat is not None:
            # Compute the absolute value of the CSI matrix
            abs_csi = np.abs(csi_mat)

            # Convert the shape of the CSI matrix to the 2D array
            # From (nTx, nRx, nSC) to (nAnt, nSC)
            abs_csi = np.reshape(abs_csi, (abs_csi.shape[0] * abs_csi.shape[1], abs_csi.shape[2]))

            # Transpose the CSI matrix and plot it
            update_plot(ax, meta_info, abs_csi.T)


if __name__ == "__main__":
    main()
