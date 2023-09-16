import os
import pandas as pd
import numpy as np
import json


def load_csi_matrix_from_file(csv_path: str):
    """
    从解析好的csv文件中读取csi矩阵，矩阵通常是一个 3 x 3 x 114 的矩阵
    Load csi matrix from the parsed csv file, the matrix is usually a 3 x 3 x 114 matrix

    @param csv_path: the path of the csv file

    @return: the csi matrix
    """

    # 验证，若csv文件不存在，则返回一个Null值
    # if the csv file does not exist, return a Null value
    if not os.path.isfile(csv_path):
        print("The csv file is not valid!")
        return None

    # 读取csv矩阵
    # read the csv matrix
    csi_pd = pd.read_csv(csv_path, sep='\t', encoding='utf-8')

    # 将CSV中的数据帧转换为CSI矩阵，这样得到数据矩阵维度就会变成 n x 3 x 3 x 114 x 2
    # Convert the data frame in the CSV to a CSI matrix, so that the dimension of the data matrix will become n x 3 x 3 x 114 x 2
    csi_matrix = csi_pd['csi_matrix']
    csi_matrix = [csi_matrix_str for csi_matrix_str in csi_matrix]  # 我们是以JSON字符串的形式存储的CSI矩阵
    csi_matrix = [np.array(json.loads(csi_matrix_str)) for csi_matrix_str in csi_matrix]  # 将JSON字符串转换为CSI矩阵
    csi_matrix = [csi[:, :, :, 0] + 1j * csi[:, :, :, 1] for csi in csi_matrix]  # 将CSI矩阵转换为复数矩阵

    # 将转换后的CSI矩阵，重新赋值给csi_pd
    # Assign the converted CSI matrix to csi_pd
    csi_pd['csi_matrix'] = csi_matrix

    return csi_pd


csi_matrix = load_csi_matrix_from_file(r"C:\Users\Seagosoft\Downloads\202309130935\csi.csv")

# print out the shape of the csi matrix
# print(csi_matrix, type(csi_matrix))
