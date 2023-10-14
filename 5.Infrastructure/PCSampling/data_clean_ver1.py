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
    # for root, dirs, files in os.walk(sys.argv[1]):
    #    for file in files:
    #        if file.endswith(".bin"):
    #            decode_csv_from_bin(os.path.join(root, file))

    # Clean the CSV file
    for root, dirs, files in os.walk(sys.argv[1]):
        # Dive into the subdirectory
        for dir in dirs:
            dir = os.path.join(root, dir)

            # search generated csv file
            for root, dirs, files in os.walk(dir):
                for file in files:
                    if file.endswith("csi.csv"):
                        print(file)
    # # Rename the pictures
    # for root, dirs, files in os.walk(sys.argv[1]):
    #     # Dive into the subdirectory
    #     for dir in dirs:
    #         dir = os.path.join(root, dir)
    #         rename_pics_in_dir(dir)
    #

    print("Start to clean the data...")
