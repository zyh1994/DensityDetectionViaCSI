from CSIDataStruct import loard_csi_from_csv
import numpy as np
import matplotlib.pyplot as plt


def db(x):
    return np.log10(x) * 10 + 30


def dbinv(x):
    return 10 ** (x / 10.0)


def plot_separated_csi(csi_matrix, mode):
    nr, nc, num_tones = csi_matrix['csi'].shape

    fig, axs = plt.subplots(nr, nc, figsize=(17, 10))
    plt.ion()

    for i in range(nr):
        for j in range(nc):
            csi_single = csi_matrix['csi'][i, j, :]
            if mode == 'amplitude':
                axs[i, j].plot(20 * np.log10(np.abs(csi_single)))
                axs[i, j].set_ylabel('SNR [dB]')
                axs[i, j].set_ylim((0, 55))
            elif mode == 'phase':
                axs[i, j].plot(np.angle(csi_single))
                axs[i, j].set_ylabel('phase (radian)')
                axs[i, j].set_ylim((-4, 4))
            axs[i, j].set_xlabel('Subcarrier index')

    plt.tight_layout()
    plt.show()


def plot_csi(csi_matrix, mode):
    # get the number of receive antennas, transmit antennas and subcarriers
    nr, nc, tones = csi_matrix['csi'].shape

    # convert the shape of the CSI matrix to (channels, subcarriers)
    csi_matrix = np.reshape(csi_matrix['csi'], (nr * nc, tones))

    # plot all the receive antennas in one figure
    if mode == "amplitude":
        plt.plot(20 * np.log10(np.abs(csi_matrix.T)))
        plt.ylabel('SNR [dB]')
        plt.ylim((0, 55))
    elif mode == "phase":
        plt.plot(np.angle(csi_matrix.T))
        plt.ylabel('phase (radian)')
        plt.ylim((-4, 4))

    plt.xlabel('Subcarrier index')
    plt.tight_layout()
    plt.show()