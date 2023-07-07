import sys
import os
import matplotlib.pyplot as plt
import numpy as np
import cv2
import time

from csi.CSVDataLoader import load_csi_from_csv
from plot.MatrixPreprocessor import preprocess_matrix, db_matrix
from plot.CSISignalPlotter import prepare_chart, update_chart


def load_frames_from_folder(folder_path: str):
    img_list = sorted(os.listdir(folder_path))
    img_data_list = []
    for img_name in img_list:
        img_path = os.path.join(folder_path, img_name)
        img_data = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
        img_data_list.append(img_data)
    return img_data_list


def draw_curve(ax, data, frame_ind, timestamp, image):
    # convert the data of CSI to 9 * 56
    data = data.reshape(9, -1)

    # get the updated chart buffer
    chart_buf = update_chart(ax, data.T)
    chart_data = np.frombuffer(chart_buf.getvalue(), dtype=np.uint8)
    chart_image = cv2.imdecode(chart_data, cv2.IMREAD_COLOR)

    # Convert the grayscale image to color
    image_color = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)

    # add the frame index to the right bottom corner of the frame
    cv2.putText(image_color, "index:" + str(frame_ind), 
                (image.shape[1] - 100, image.shape[0] - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5,
                (255, 255, 255), 1, cv2.LINE_AA)

    # add the timestamp the left top corner of the frame, flowing the frame index
    cv2.putText(image_color, timestamp, (10, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5,
                (255, 255, 255), 1, cv2.LINE_AA)

    # Resize the chart image to match the size of the image
    chart_image = cv2.resize(chart_image, (image.shape[1], image.shape[0]))

    # Combine the color image and the chart image horizontally
    combined_image = cv2.hconcat([image_color, chart_image])

    return combined_image

    # # Set the alpha value for the chart image
    # alpha = 0.5

    # # Combine the image and the chart image with transparency
    # combined_image = cv2.addWeighted(image_color, 1-alpha, chart_image, alpha, 0)

    # # Add the timestamp to the top left corner of the frame
    # cv2.putText(combined_image, timestamp, (0, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5,
    #             (255, 255, 255), 1, cv2.LINE_AA)

    # return combined_image


def main(folder_path: str, start_frame: int = 0, end_frame: int = 0):
    # if the folder path is not valid, exit the program
    if not os.path.isdir(folder_path):
        print("The folder path is not valid!")
        sys.exit(1)

    # assemble the full path of the folder
    img_path = os.path.join(folder_path, "png")

    # load the image list
    frames_list = load_frames_from_folder(img_path)

    # assemble the full path of the csi file
    csi_file_path = os.path.join(folder_path, "csi_data.csv")

    # load the csi data from the csv file
    csi_info_list = load_csi_from_csv(csi_file_path)

    # prepare the chart
    ax = prepare_chart("CSI Signal")

    # if the end_second is not specified, set it to the last second
    if end_frame == 0:
        end_frame = len(frames_list)
    else:
        # if the end_frame is larger than the length of the frames_list, set it to the last frame
        end_frame = min(end_frame, len(frames_list))

    if start_frame >= end_frame:
        print("The start frame index is larger than the end frame index!")
        sys.exit(1)

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
        img = draw_curve(ax, csi_db_matrix, ind, timestamp, img)

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
