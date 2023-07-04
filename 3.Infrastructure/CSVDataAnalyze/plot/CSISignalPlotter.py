import matplotlib.pyplot as plt
from io import BytesIO


def prepare_chart(title):
    # Set up real-time plotting
    plt.switch_backend('Agg')  # 设置Matplotlib的后端为非交互式模式

    # Create a figure and an axis
    fig, ax = plt.subplots()

    # Change the figure title
    # fig.canvas.manager.set_window_title(title)

    return ax


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