import os
import cv2


def load_frames_from_folder(folder_path: str):
    """
    当CSI的BIN文件解压后，会生成一个名叫JPN或者PNG的文件夹，并且图片会以Millisecond为单位命名。
    该函数的作用就是负责从给定的路径中读取所有的图片，并且按照时间先后返回一个图片数据的列表。

    Since we decoded the CSI BIN file, a folder named JPN or PNG will be generated, and the images will be named in Millisecond.
    The function is responsible for reading all the images from the given path and returning a list of image data in chronological order.
    """

    # 获取文件夹中的所有图片，并排序
    # Get all the images in the folder and sort them
    img_list = sorted(os.listdir(folder_path))

    # 用于存储图片数据的列表
    # List for storing image data    
    img_data_list = []
    for img_name in img_list:

        # 组装图片的完整路径
        # Assemble the full path of the image
        img_path = os.path.join(folder_path, img_name)

        # 读取图片数据
        # Read image data
        img_data = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
        
        # 将图片数据添加到列表中
        # Add image data to the list
        img_data_list.append(img_data)
    
    return img_data_list
