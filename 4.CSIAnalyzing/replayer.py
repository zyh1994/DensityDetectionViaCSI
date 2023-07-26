import sys
import os
import matplotlib.pyplot as plt
import cv2
import time

from csi.CSVDataLoader import load_csi_from_csv
from plot.LinearSignalChart import gen_linear_chart
from utilities.MatrixPreprocessor import preprocess_matrix, db_matrix
from utilities.ImageLoader import load_frames_from_folder


def prepare_chart():
    # Set up real-time plotting
    plt.switch_backend('Agg')  # 设置Matplotlib的后端为非交互式模式

    # Create a figure and an axis
    _, ax = plt.subplots()

    return ax


def time_window(start_frame, end_frame, frames_list):
    if end_frame == 0:
        end_frame = len(frames_list)
    else:
        # if the end_frame is larger than the length of the frames_list, set it to the last frame
        end_frame = min(end_frame, len(frames_list))

    if start_frame >= end_frame:
        print("The start frame index is larger than the end frame index!")
        sys.exit(1)

    return start_frame, end_frame


def draw_charts(ax, data, frame_ind, timestamp, image):
    # generate linear chart from the data
    chart_image = gen_linear_chart(data, ax)

    # Convert the grayscale image to color
    image_color = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)

    # add the frame index to the right bottom corner of the frame
    cv2.putText(image_color, 
                "index:" + str(frame_ind), 
                (image.shape[1] - 100, image.shape[0] - 10), 
                cv2.FONT_HERSHEY_SIMPLEX, 
                0.5,
                (255, 255, 255), 
                1, 
                cv2.LINE_AA)

    # add the timestamp the left top corner of the frame, flowing the frame index
    cv2.putText(image_color, 
                timestamp, 
                (10, 20), 
                cv2.FONT_HERSHEY_SIMPLEX, 
                0.5,
                (255, 255, 255), 
                1, 
                cv2.LINE_AA)
    
    # Check if the chart_image is empty
    if chart_image is not None:
        # Resize the chart image to match the size of the image
        chart_image = cv2.resize(chart_image, (image.shape[1], image.shape[0]))

        # Combine the color image and the chart image horizontally
        combined_image = cv2.hconcat([image_color, chart_image])

        return combined_image
    else:
        # If chart_image is empty, return only the color image
        return image_color


def main(folder_path: str, start_frame: int = 0, end_frame: int = 0):
    # if the folder path is not valid, exit the program
    if not os.path.isdir(folder_path):
        print("The folder path is not valid!")
        sys.exit(1)

    # load the image list
    frames_list = load_frames_from_folder(os.path.join(folder_path, "png"))
    if frames_list is None or len(frames_list) == 0:
        print("The frames_list is empty!")
        sys.exit(1)

    # load the csi data from the csv file
    csi_info_list = load_csi_from_csv(os.path.join(folder_path, "csi_data.csv"))
    if csi_info_list is None or len(csi_info_list) == 0:
        print("The csi_info_list is empty!")
        sys.exit(1)
    
    # prepare the chart
    ax = prepare_chart()

    # if the end_second is not specified, set it to the last second
    start_frame, end_frame = time_window(start_frame, end_frame, frames_list)

    # display each image frame by frame
    for ind in range(start_frame, end_frame):
        # get the image data
        img = frames_list[ind]

        # get the csi data
        csi_data = csi_info_list[ind]

        # get the amplitude matrix
        csi_db_matrix = db_matrix(preprocess_matrix(csi_data.data()))

        # get the long timestamp: csi_data.meta().timestamp
        # convert the long timestamp to the format of "YYYY-MM-DD HH:MM:SS" 
        # Seattle time zone: UTC-7
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S", 
                                  time.localtime(csi_data.meta().timestamp / 1000 - 7 * 3600))

        # draw the curve
        img = draw_charts(ax, csi_db_matrix, ind, timestamp, img)

        # Display the image
        cv2.imshow("CSI Signal", img)

        # if key pressed is 'q', exit the program
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Close the matplotlib figure
    plt.close()


if __name__ == '__main__':

    # if user prints the usage
    if len(sys.argv) == 2 and sys.argv[1] == "-h":
        print("Usage: python parser.py [csi_folder_path] [start frame index] [end frame index]]")
        sys.exit(0)

    if len(sys.argv) == 3:
        main(sys.argv[1], int(sys.argv[2]))
    elif len(sys.argv) == 4:
        main(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
    else:
        main(sys.argv[1])
