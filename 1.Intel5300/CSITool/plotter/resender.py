import socket
import struct
import time


def setup_tcp_client():

    # create a socket
    sock_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # set the server address and port number
    server_address = ('127.0.0.1', 8090)

    # connect to the server
    sock_fd.connect(server_address)

    # return the socket
    return sock_fd


def main():

    # Open the client socket
    sock_fd = setup_tcp_client()

    # Open the "csi.dat" file for reading
    with open("csi.dat", "rb") as in_file:

        # Read and process each frame of CSI data from the file
        while True:
            # Read the length of bytes in the data
            loaded_bytes = in_file.read(2)
            if not loaded_bytes:
                break

            # Keep the loaded length in hex
            data_len_hex = loaded_bytes

            # Convert the data to the short type
            data_len = struct.unpack('>H', loaded_bytes)[0]

            # Load the reset bytes of the data frame
            loaded_bytes = in_file.read(data_len)

            # Send the data length to the server
            sock_fd.send(data_len_hex)

            # Send the data to the server
            sock_fd.send(loaded_bytes)

            # Print out the debug information
            print(f"Data length in hex: {data_len_hex.hex()} md5: {md5_value.hex()}")

            # Sleep for 0.2 seconds
            time.sleep(0.2)


if __name__ == '__main__':

    # Read data from the "csi.dat" file and process each CSI frame
    main()
