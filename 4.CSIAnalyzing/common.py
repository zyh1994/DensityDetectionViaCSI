import os
import sys

from csi.CSVDataLoader import load_csi_from_csv


def load_csv_from_folder(path: str):
    # if the folder path is not valid, exit the program
    if not os.path.isdir(path):
        print("The folder path is not valid!")
        sys.exit(1)

    # load the csi data from the csv file
    csi_info_list = load_csi_from_csv(os.path.join(path, "csi_data.csv"))
    if csi_info_list is None or len(csi_info_list) == 0:
        print("The csi_info_list is empty!")
        sys.exit(1)

    return csi_info_list
