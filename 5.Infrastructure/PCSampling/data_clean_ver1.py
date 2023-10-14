from tools.YoloV8Mark import recognize_human_bbx
from tools.RenamePictures import rename_pics_in_dir
from tools.RemoveLowDimData import clean_csv_by_given_path
from tools.DecodeCSVFromBin import decode_csv_from_bin

import sys
import os


if __name__ == "__main__":

    # print out usage
    if len(sys.argv) == 2 and sys.argv[1] == "-h":
        print("Usage: python data_clean_ver1.py [dir]")
        print("Example: python data_clean_ver1.py ./data")
        exit(0)

    # Decode the CSV from bin
    for root, dirs, files in os.walk(sys.argv[1]):
        for file in files:
            if file.endswith('csi.bin'):
                bin_file_path = os.path.join(root, file)
                if not os.path.exists(bin_file_path):  # process first-level csi.bin only
                    continue
                print("Decode the CSV from bin: " + bin_file_path)
                decode_csv_from_bin(os.path.join(root, file))

    # Clean the CSV file
    for root, dirs, files in os.walk(sys.argv[1]):
        # Dive into the subdirectory
        for dir in dirs:
            csv_path = os.path.join(root, dir, "csi.csv")
            if os.path.exists(csv_path):
                print("Clean the CSV file: " + csv_path)
                clean_csv_by_given_path(csv_path)

    # Rename the pictures
    for root, dirs, files in os.walk(sys.argv[1]):
        jpg_path = os.path.join(root, "jpg")
        if os.path.exists(jpg_path):
            # Rename the pictures
            rename_pics_in_dir(root)
            # Rename the folder
            os.rename(jpg_path, os.path.join(root, "jpg_processed"))

    # Recognize the human BBX
    for root, dirs, files in os.walk(sys.argv[1]):
        jpg_path = os.path.join(root, "jpg_processed")
        if os.path.exists(jpg_path):
            # Recognize the human BBX
            recognize_human_bbx(root)


    print("Start to clean the data...")
