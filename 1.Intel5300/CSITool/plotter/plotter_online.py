import socket
import struct

from plotter_base import *


def setup_client_side():
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect the socket to the port where the server is listening
    server_address = ('localhost', 8090)

    # Connect to the server
    sock.connect(server_address)

    return sock


def main():

    # Prepare dynamically plotter
    ax = prepare_chart("CSI Online")

    # Get the client socket
    cli_sock = setup_client_side()

    while True:

        # Two bytes for the length of the data
        loaded_bytes = cli_sock.recv(2)

        # If the socket is closed, then reconnect to the server
        if not loaded_bytes:
            print("The TCP socket is closed by the remote, try to reconnect")
            cli_sock = setup_client_side()
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
            update_chart(ax, meta_info, abs_csi.T)


if __name__ == "__main__":
    main()
