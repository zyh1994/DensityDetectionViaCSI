import sys
import os
import matplotlib.pyplot as plt
import numpy as np

from csi.CSVDataLoader import load_csi_from_csv


def load_img_from_folder(folder_path: str):
    img_list = sorted(os.listdir(folder_path))
    return img_list


def main(folder_path: str):

    # if the folder path is not valid, exit the program
    if not os.path.isdir(folder_path):
        print("The folder path is not valid!")
        sys.exit(1)
    
    # assemble the full path of the folder
    img_path = os.path.join(folder_path, "png")

    # load the image list
    img_list = load_img_from_folder(img_path)

    # assemble the full path of the csi file
    csi_file_path = os.path.join(folder_path, "csi_data.csv")

    # load the csi data from the csv file
    csi_info_list = load_csi_from_csv(csi_file_path)


    print(len(img_list), len(csi_info_list))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python parser.py [csi_folder_path]")
        sys.exit(1)

    main(sys.argv[1])