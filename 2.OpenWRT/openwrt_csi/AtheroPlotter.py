import numpy as np
import matplotlib.pyplot as plt
import matplotlib

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




# def plot_single_csi(csi_matrix):
#     fig, axs = plt.subplots(2, 2, figsize=(17, 10))
#     plt.ion()
#
#     csi = csi_matrix['csi'][0, :, :]
#     axs[0, 0].plot(20 * np.log10(np.abs(csi)))
#     axs[0, 0].set_ylabel('CSI Amplitude [dB]')
#     axs[0, 0].set_ylim((30, 50))
#     axs[0, 0].set_xlabel('Subcarrier index')
#
#     axs[0, 1].plot(np.unwrap(np.angle(csi)))
#     axs[0, 1].set_ylabel('unwrapped phase (radian)')
#     axs[0, 1].set_ylim((-4, 4))
#     axs[0, 1].set_xlabel('Subcarrier index')
#
#     scaled_csi = get_scaled_csi(csi_matrix)
#     axs[1, 0].plot(scaled_csi[0, :, :])
#     axs[1, 0].set_ylabel('scaled RSS [dBm]')
#     axs[1, 0].set_xlabel('Subcarrier index')
#
#     axs[1, 1].plot(np.unwrap(np.angle(csi[:, :2])))
#     axs[1, 1].set_ylabel('unwrapped phase (radian)')
#     axs[1, 1].set_ylim((-6, 6))
#     axs[1, 1].set_xlabel('Subcarrier index')
#
#     plt.tight_layout()
#     plt.show()


def get_scaled_csi(csi_matrix):
    csi = csi_matrix['csi']
    rssi1 = csi_matrix['rssi1']
    rssi2 = csi_matrix['rssi2']
    rssi3 = csi_matrix['rssi3']

    csi_squared = np.abs(csi) ** 2
    csi_scaled = np.zeros_like(csi)

    if rssi1 != 128:
        signal_level = rssi1 - 95
        rssi_mag = dbinv(signal_level)
        csi_pwr = np.sum(csi_squared[0])
        csi_scaled[0] = 10 * np.log10(csi_squared[0] / csi_pwr * rssi_mag)

    if rssi2 != 128:
        signal_level = rssi2 - 95
        rssi_mag = dbinv(signal_level)
        csi_pwr = np.sum(csi_squared[1])
        csi_scaled[1] = 10 * np.log10(csi_squared[1] / csi_pwr * rssi_mag)

    if rssi3 != 128:
        signal_level = rssi3 - 95
        rssi_mag = dbinv(signal_level)
        csi_pwr = np.sum(csi_squared[2])
        csi_scaled[2] = 10 * np.log10(csi_squared[2] / csi_pwr * rssi_mag)

    return csi_scaled


def db(x):
    return np.log10(x) * 10 + 30


def dbinv(x):
    return 10 ** (x / 10.0)
