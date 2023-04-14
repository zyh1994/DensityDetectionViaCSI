import numpy as np
import matplotlib.pyplot as plt
import socket
import struct
import hashlib


def setup_server_side():

    # Create a TCP/IP server socket for the client to connect to
    server_address = ('127.0.0.1', 8090)

    # Create a TCP/IP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Bind the socket to the port and the address
    server_socket.bind(server_address)

    # Listen for incoming connections
    server_socket.listen(1)

    # Return the server side socket
    return server_socket


def main():

    # Get the server side socket
    serv_sock = setup_server_side()

    while True:
        # Wait for a client to connect
        cli_sock, cli_addr = serv_sock.accept()

        if cli_sock is not None:
            print("Client connected from: ", cli_addr)

        # Receive data from the client
        try:
            while True:

                # If the client socket is closed, break the loop
                if cli_sock.close():
                    break

                # Receive the length of the message
                raw_data_len = cli_sock.recv(2)
                if not raw_data_len:
                    print("Connection closed by client")
                    break

                # Convert the data to the short type
                data_len = struct.unpack('>H', raw_data_len)[0]

                # Receive the data
                raw_data = cli_sock.recv(data_len)
                if not raw_data:
                    print("Connection closed by client")
                    break

                # TODO: Processing the data and plot it to the chart

                # Print out the debug information
                md5_value = hashlib.md5(raw_data).digest()
                print(f"Data length in hex: {raw_data_len.hex()} md5: {md5_value.hex()}")

        except ConnectionAbortedError:
            print('Connection aborted')
        except ConnectionResetError:
            print('Connection reset')
        except socket.timeout:
            print('Connection timed out')
        finally:
            # Close the socket
            cli_sock.close()


if __name__ == "__main__":
    main()
