from tools.YoloV8Mark import recognize_human_bbx
from tools.RenamePictures import rename_pics_in_dir
from tools.RemoveLowDimData import clean_csv_in_dir
from tools.DecodeCSVFromBin import decode_csv_from_bin

import sys
import os


if __name__ == "__main__":

    # print out usage
    if len(sys.argv) == 2 and sys.argv[1] == "-h":
        print("Usage: python data_clean_ver1.py [dir]")
        print("Example: python data_clean_ver1.py ./data")
        exit(0)

    for root, dirs, files in os.walk(sys.argv[1]):
        for file in files:

            # Decode the CSV from bin
            if file.endswith(".bin"):
                decode_csv_from_bin(os.path.join(root, file))

    print("Start to clean the data...")
