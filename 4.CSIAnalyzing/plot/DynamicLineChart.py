from io import BytesIO

import cv2
import matplotlib.pyplot as plt
import numpy as np


class DynamicLineChart:

    def __init__(self, x_label="x label", y_label="y label", chart_title="title",
                 interval=-1, legend_loc='upper left', show_legend=False):

        # Set up the interval for dynamic plotting
        self.interval = interval

        # Create a figure and axis for the plot
        self.fig, self.ax = plt.subplots()

        # Set up the labels and title
        self.ax.set_xlabel(x_label)
        self.ax.set_ylabel(y_label)
        self.ax.set_title(chart_title)

        # Set up the legend
        self.legend_loc = legend_loc
        self.show_legend = show_legend
        self.sub_labels = []

        # Set up the data storage
        self.data_storage = []

    def insert_data(self, data, label=None):
        # Chck the shape of the data, and convert it to a one-dimensional array if necessary
        if len(data.shape) > 1:
            data = data.reshape(-1)

        # Check if the label is specified
        if label is not None:
            self.sub_labels.append(label)

        # Insert the data into the data storage
        self.data_storage.append(data)

    def update_data(self, data, channels):
        # Check the shape of the data, and convert it to a two-dimensional array if necessary
        if len(data.shape) == 1:
            data = data.reshape(-1, channels)

        # Update the data in the data storage
        for i in range(channels):
            self.data_storage.append(data[:, i])
            self.sub_labels.append(f"Channel {i + 1}")

    def clear_data(self):
        if len(self.data_storage) > 0:
            self.data_storage.clear()

        if len(self.sub_labels) > 0:
            self.sub_labels.clear()

    def show_plot(self, output=False, display=True):

        # Clear the previous plot to avoid overlap
        self.ax.clear()

        # Update the plot with lines and labels for the legend
        for i, data in enumerate(self.data_storage):
            if self.show_legend:
                self.ax.plot(data, label=self.sub_labels[i])
            else:
                self.ax.plot(data)

        # Add the legend with the specified location
        if self.show_legend:
            self.ax.legend(loc=self.legend_loc)

        # Set the interval for dynamic plotting
        if display:
            if self.interval > 0:
                plt.pause(self.interval)
            else:
                plt.show()

        if output:
            # Use the BytesIO object to create a PNG image of the plot
            buffer = BytesIO()
            plt.savefig(buffer, format='png')
            buffer.seek(0)

            # Convert the PNG buffer to a numpy array
            chart_data = np.frombuffer(buffer.getvalue(), dtype=np.uint8)

            # Decode the PNG numpy array
            chart_image = cv2.imdecode(chart_data, cv2.IMREAD_COLOR)

            # Return the image
            return chart_image

        else:
            return None

    # def show_plot(self):
    #     # Display the plot
    #     plt.show()

    # def close_plot(self):
    #     # Close the figure to release memory resources
    #     plt.close(self.fig)


if __name__ == "__main__":

    # generate some random data
    data = np.random.randint(0, 10, size=(10, 5))

    # create the chart
    chart = DynamicLineChart()

    # update the chart
    chart.update_data(data, 5)

    # Display the plot (optional, for visual inspection)
    chart.show_plot()

