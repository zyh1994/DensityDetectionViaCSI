import matplotlib.pyplot as plt
from io import BytesIO

import numpy as np
import cv2


def update_chart(ax, csi_matrix):
    # Clear the previous plot
    ax.clear()

    # Plot the CSI matrix
    ax.plot(csi_matrix)

    # Set the legend and the axis labels
    ax.set_xlabel('Subcarrier index')
    ax.set_ylabel('SNR [dB]')

    # Save the plot to a BytesIO object
    buffer = BytesIO()
    plt.savefig(buffer, format='png')
    buffer.seek(0)

    # Return the BytesIO object
    return buffer


def gen_linear_chart(data: np.ndarray, ax):
    # convert the data to a two-dimensional array
    _, _, nsubcarrier = data.shape
    data = data.reshape(-1, nsubcarrier)

    # get the updated chart buffer
    chart_buf = update_chart(ax, data.T)
    chart_data = np.frombuffer(chart_buf.getvalue(), dtype=np.uint8)
    chart_image = cv2.imdecode(chart_data, cv2.IMREAD_COLOR)

    return chart_image