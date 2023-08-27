import struct
import sys
import os
import shutil

from video.VideoWriter import VideoWriter
from csi.CSIConverter import CSIConverter
from data.BinDataStruct_v1 import OpenCVFrameData, CSIDataFrameData


def parse_image_data(cv_data: bytes, cv_data_len: int,
                     width: int = 320, height: int = 180, fps: int = 30):
    
    # Video writer for generating the mp4 file
    video_writer = VideoWriter("cv_data.mp4", fps=fps, width=width, height=height)

    # Use a list to store the CV data frames
    cv_data_frames = []

    # Parse the CV data
    anchor = 0
    while cv_data_len > 0:

        # Get the size of the data frame
        data_frame_size = struct.unpack('<Q', cv_data[anchor:anchor + 8])[0]

        # Get the data frame
        data_frame = cv_data[anchor: anchor + data_frame_size]

        # Parse the data frame
        cv_data_frame = OpenCVFrameData(data_frame)

        # print(cv_data_frame.width, cv_data_frame.height, cv_data_frame.channels,
        #       cv_data_frame.frame_size, cv_data_frame.raw_size, len(cv_data_frame.raw))

        # Add the data frame to the list
        cv_data_frames.append(cv_data_frame)

        # Update the anchor and the data length
        anchor += data_frame_size
        cv_data_len -= data_frame_size

    # Save the CV data to a mp4 file
    for frame in cv_data_frames:
        video_writer.write(frame.raw, frame.timestamp)


def parse_csi_data(csi_data: bytes, csi_data_len: int):

    # Create a CSI converter
    csi_converter = CSIConverter("csi_data.csv")

    # Use a list to store the CSI data frames
    csi_data_frames = []

    # Parse the CSI data
    anchor = 0
    while csi_data_len > 0:

        # Get the size of the data frame
        data_frame_size = struct.unpack('<Q', csi_data[anchor:anchor + 8])[0]

        # Get the data frame
        data_frame = csi_data[anchor: anchor + data_frame_size]

        # Parse the data frame
        csi_data_frame = CSIDataFrameData(data_frame)

        # Add the data frame to the list
        csi_data_frames.append(csi_data_frame)

        # Update the anchor and the data length
        anchor += data_frame_size
        csi_data_len -= data_frame_size

    # save the CSI data to a csv file
    for frame in csi_data_frames:
        csi_converter.write(frame.raw, frame.raw_size, frame.timestamp)
    

def parse_bin_file(csi_raw_video: str, width: int = 320, height: int = 180, fps: int = 30):
    # Open the file
    f = open(csi_raw_video, 'rb')

    # If the file is not opened successfully, then output the error message and return
    if not f:
        raise Exception("couldn't open file {}".format(csi_raw_video))

    # Move the file pointer back to the beginning of the file to start reading the file
    f.seek(0, 0)

    # The bin file is roughly composed of four parts, the first is the total length of 
    # CV data and the total length of CSI data composed of two size_t.
    cv_total_size = struct.unpack('<Q', f.read(8))[0]
    csi_total_size = struct.unpack('<Q', f.read(8))[0]

    # The second part is the CV data, which is the raw video data
    cv_data = f.read(cv_total_size)

    # The third part is the CSI data, which is the raw CSI data
    csi_data = f.read(csi_total_size)

    # Close the file first
    f.close()

    # Return the data
    return {
        "cv_data": cv_data,
        "cv_data_len": cv_total_size,
        "csi_data": csi_data,
        "csi_data_len": csi_total_size
    }


def move_files_to_folder(path: str):
    # Extract the file name from the path
    file_name = os.path.basename(path)

    # Remove the file extension from the name
    folder = os.path.splitext(file_name)[0]

    # Create a folder under the same directory as the file
    folder = os.path.join(os.path.dirname(path), folder)

    # Create a folder to store the files
    if not os.path.exists(folder):
        os.mkdir(folder)

    # Create a folder to store the pictures
    pictures_folder = os.path.join(folder, 'png')
    if not os.path.exists(pictures_folder):
        os.mkdir(pictures_folder)

    # Move the files to the folder
    for file in os.listdir():
        if file.endswith('.mp4'):
            shutil.move(file, folder)
        if file.endswith('.csv'):
            shutil.move(file, folder)
        if file.endswith('.png'):
            shutil.move(file, pictures_folder)

    # Move the bin file to the folder
    shutil.move(path, folder)

    # Move the related avi file to the folder
    if os.path.exists(os.path.splitext(path)[0] + '.avi'):
        shutil.move(os.path.splitext(path)[0] + '.avi', folder)

    # Print out the folder name
    print("The files are moved to the folder: {}".format(folder))


if __name__ == '__main__':

    # print out the usage
    if len(sys.argv) == 2 and sys.argv[1] == '-h':
        print("Usage: python ParseCSIData.py <csi_video_file> <width> <height> <fps>")
        sys.exit(1)

    # Get the data from the bin file
    if len(sys.argv) == 5:
        csv_info = parse_bin_file(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]))
    elif len(sys.argv) == 4:
        csv_info = parse_bin_file(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
    else:
        csv_info = parse_bin_file(sys.argv[1])

    # Parse the image data
    parse_image_data(csv_info["cv_data"], csv_info["cv_data_len"])

    # Parse the CSI data
    parse_csi_data(csv_info["csi_data"], csv_info["csi_data_len"])

    # Move the files to a folder
    move_files_to_folder(sys.argv[1])
