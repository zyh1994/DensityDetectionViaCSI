import os
import cv2


def load_frames_from_folder(folder_path: str):
    img_list = sorted(os.listdir(folder_path))
    img_data_list = []
    for img_name in img_list:
        img_path = os.path.join(folder_path, img_name)
        img_data = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
        img_data_list.append(img_data)
    return img_data_list