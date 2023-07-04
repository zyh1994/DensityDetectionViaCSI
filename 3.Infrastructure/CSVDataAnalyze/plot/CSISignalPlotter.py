import numpy as np
import matplotlib.pyplot as plt


def update_dynamic_chart(ax, csi_matrix, chart_title, x_label, y_label, pause_time=0.01):
    # Clear the previous plot
    ax.clear()

    # Set the legend and the axis labels
    ax.set_xlabel(x_label)
    ax.set_ylabel(y_label)

    # Set the title
    ax.set_title(chart_title)

    # Plot the CSI matrix
    ax.plot(csi_matrix)

    # Pause for 0.01 second
    plt.pause(0.01)


def update_dynamic_vid_chart(ax, img, csi_matrix, chart_title, x_label, y_label, pause_time=0.01):
    # Clear the previous plot
    ax.clear()

    # Set the legend and the axis labels
    ax.set_xlabel(x_label)
    ax.set_ylabel(y_label)

    # Set the title
    ax.set_title(chart_title)

    # Plot the CSI matrix
    ax.imshow(img, cmap='gray')

    # Pause for 0.01 second
    plt.pause(0.01)