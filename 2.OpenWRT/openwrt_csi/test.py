from AtheroPlotter import *
import time

from ReadLogFile import read_log_file


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


def main():
    csi_frames = read_log_file('csi.dat')

    # prepare the chart
    ax = prepare_chart("CSI Amplitude")

    # plot each frame in the signal frame
    for frame in csi_frames:

        # get the number of receive antennas, transmit antennas and subcarriers
        nr, nc, tones = frame['csi'].shape

        # convert the shape of the CSI matrix to (channels, subcarriers)
        csi_matrix = np.reshape(frame['csi'], (nr * nc, tones))

        # plot the amplitude of the CSI
        update_chart(ax, csi_matrix)

        # sleep for 0.5 second
        plt.pause(0.5)


if __name__ == '__main__':
    main()