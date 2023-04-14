import binascii
import sys


if __name__ == '__main__':

    # Check the number of arguments
    if len(sys.argv) != 2:
        print('Usage: python hex_reader.py <csi.cdat>')
        sys.exit(1)

    # Read the filename from the command line
    filename = sys.argv[1]

    # Read the file contents
    with open(filename, 'rb') as f:
        contents = f.read()
        hex_contents = binascii.hexlify(contents).decode()
        separated_hex = ' '.join(hex_contents[i:i+2] for i in range(0, len(hex_contents), 2))
        print(separated_hex)
