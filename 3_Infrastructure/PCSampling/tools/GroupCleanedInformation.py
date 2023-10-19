import pandas as pd
import os


def load_timestamps_from_jpg_dir(jpg_dir):
    for root, dirs, files in os.walk(jpg_dir):
        if len(files) > 0:
            return [int(file.split('.')[0]) for file in files]