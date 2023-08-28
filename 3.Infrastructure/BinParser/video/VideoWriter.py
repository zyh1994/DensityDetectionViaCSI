import cv2
import numpy as np
import time


class VideoWriter:

    def __init__(self, filename, fps, width, height):
        self.filename = filename
        self.fps = fps
        self.width = width
        self.height = height

        # use mp4 codec 
        fourcc = cv2.VideoWriter_fourcc(*'mp4v')

        # create a video writer
        self.video_writer = cv2.VideoWriter(self.filename, fourcc, self.fps, (self.width, self.height), False)

    def write(self, raw_frame, timestamp):

        # convert the raw frame to a numpy array
        frame = np.frombuffer(raw_frame, dtype=np.uint8)

        # reshape the numpy array to a grayscale image
        frame = frame.reshape((self.height, self.width))

        # save the image to a file and using the timestamp as the filename
        cv2.imwrite(str(timestamp) + '.png', frame)

        # convert the timestamp (long integer) to yyyy-mm-dd hh:mm:ss
        # timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(timestamp))
        timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(timestamp / 1000))  # timestamp is in milliseconds

        # add the timestamp the left top corner of the frame
        cv2.putText(frame, str(timestamp), (0, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1, cv2.LINE_AA)

        # if the frame is not empty and the video writer is opened
        if frame is not None and self.video_writer.isOpened():
            # write the frame to the video file
            self.video_writer.write(frame)

        # if the video writer is not opened
        elif not self.video_writer.isOpened():
            print("VideoWriter is not opened")
            exit(1)

        # if the frame is empty
        elif frame is None:
            print("Frame is empty")
            exit(1)

        else:
            print("Unknown error")
            exit(1)

    def release(self):
        self.video_writer.release()
