from AtheroPlotter import *
import struct
from VideoWriter import VideoWriter

from CSIConverter import CSIConverter


def prepare_chart(title):
    # Set up real-time plotting
    plt.ion()

    # Create a figure and an axis
    fig, ax = plt.subplots()

    # Change the figue title
    fig.canvas.manager.set_window_title(title)

    return ax


def update_chart(ax, csi_matrix):
    # Clear the previous plot
    ax.clear()

    # Plot the CSI matrix
    ax.plot(20 * np.log10(np.abs(csi_matrix.T)))

    # Set the legend and the axis labels
    ax.set_xlabel('Subcarrier index')
    ax.set_ylabel('SNR [dB]')

    # Pause for 0.01 second
    plt.pause(0.01)


def data_separator(csi_raw_video: str):
    # Open the file
    f = open(csi_raw_video, 'rb')

    # If the file is not opened successfully, then output the error message and return
    if not f:
        raise Exception("couldn't open file {}".format(csi_raw_video))

    # Move the file pointer to the end of the file to get the length of the file
    f.seek(0, 2)
    len = f.tell()
    print('file length is: {}'.format(len))

    # Move the file pointer back to the beginning of the file to start reading the file
    f.seek(0, 0)

    # Now we have to read the frame from the file. And the frame's structure is as follows:
    #  total_size | frame_size | csi_size | timestamp | frame       | csi_data | \r\n
    #  8 bytes    | 8 bytes    | 8 bytes  | 8 bytes   | frame_size  | csi_size | 2 bytes

    cur = 0     # Current position
    count = 0   # Number of data blocks read

    # Endian format
    endian_format = '<'

    # Create a video writer 320 x 240, 30 fps
    csi_video = VideoWriter('csi_video.mp4', 30, 320, 240)

    # Create a file to store the csi matrix
    csi_converter = CSIConverter("csi_data.csv")

    # Read the file in a loop until the end of the file
    while cur < (len - 32):
        # Read the length of the current block
        total_size = struct.unpack(endian_format + 'Q', f.read(8))[0]
        cur += 8
        # print('total_size is: {}'.format(total_size))

        # If the length of the current block is greater than the remaining file length, then break the loop
        if (cur + total_size) > len:
            break

        # Read the length of the frame
        frame_size = struct.unpack(endian_format + 'Q', f.read(8))[0]
        cur += 8
        # print('frame_size is: {}'.format(frame_size))

        # Read the length of the csi matrix
        csi_size = struct.unpack(endian_format + 'Q', f.read(8))[0]
        cur += 8
        # print('csi_size is: {}'.format(csi_size))

        # Read the timestamp
        timestamp = struct.unpack(endian_format + 'Q', f.read(8))[0]
        cur += 8
        # print('timestamp is: {}'.format(timestamp))

        # Read the frame
        frame = f.read(frame_size)
        cur += frame_size

        # Read the csi matrix
        csi_raw = f.read(csi_size)
        cur += csi_size

        # Read the last two bytes
        last_two_bytes = f.read(2)
        cur += 2

        # is the last two bytes equal to '\r\n'?
        if last_two_bytes != b'\r\n':
            print('last two bytes is not equal to \\r\\n')
            break

        # Write the frame to the video file
        csi_video.write(frame, timestamp)

        # try to read the csi matrix
        csi_converter.write(csi_raw, csi_size, timestamp)

        # Increase the number of data blocks read
        count += 1

    # Close the file
    f.close()

    # Close the video file
    csi_video.release()

    # Close the csi matrix file
    # csi_raw.close()

    return None, None


# def main(raw_data: str):
#
#     # get the video and csi matrix
#     csi_video, csi_raw = data_separator(raw_data)
#
#     # get the csi matrix from the raw data
#     csi_frames = read_log_file(csi_raw)
#
#     # prepare the chart
#     ax = prepare_chart("CSI Amplitude")
#
#     # plot each frame in the signal frame
#     for frame in csi_frames:
#
#         # get the number of receive antennas, transmit antennas and subcarriers
#         nr, nc, tones = frame['csi'].shape
#
#         # convert the shape of the CSI matrix to (channels, subcarriers)
#         csi_matrix = np.reshape(frame['csi'], (nr * nc, tones))
#
#         # plot the amplitude of the CSI
#         update_chart(ax, csi_matrix)
#
#         # sleep for 0.5 second
#         plt.pause(0.5)


if __name__ == '__main__':
    # if len(sys.argv) != 2:
    #     print("Usage: python PlayCSIVideo.py <csi_video_file>")
    #     sys.exit(1)

    data_separator(r"C:\Users\Seagosoft\Downloads\csi_videoc.dat")
