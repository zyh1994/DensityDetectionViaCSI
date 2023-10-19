import os
import pandas as pd
import numpy as np
import json
import sys


def load_images_from_folder(folder_path: str):
    image_dict = {}

    for root, dirs, files in os.walk(folder_path):
        for file in files:
            if file.endswith('jpg') or file.endswith('png'):
                image_dict[file] = os.path.join(root, file)

    return image_dict


def do_cleaning(csv_path: str):
    # Scan the directory and get all the image files
    for root, dirs, files in os.walk(csv_path):

        # Load the images
        images = None
        for dir in dirs:
            if dir == 'jpg' or dir == 'png':
                images = load_images_from_folder(os.path.join(root, dir))

        # Load the CSV file
        timestamp_list = None
        for file in files:
            if file.endswith('csi.csv'):
                csv_pd = pd.read_csv(os.path.join(root, file), sep='\t', encoding='utf-8')
        
                # Get the timestamp list from the CSV file
                timestamp_list = csv_pd['timestamp'].tolist()

        # Get the first and last timestamp
        first_timestamp = int(timestamp_list[0])
        last_timestamp = int(timestamp_list[-1])

        gap = last_timestamp - first_timestamp
        print ('gap: ', gap)


if __name__ == "__main__":

    # print out the usage
    if len(sys.argv) == 2 and sys.argv[1] == '-h': 
        print("Usage: python pic_clean_v1.py <root_folder>")
    
    # Move the files to a folder
    do_cleaning(sys.argv[1])
