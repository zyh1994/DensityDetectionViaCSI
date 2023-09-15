import os
import pandas as pd
import numpy as np


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
    csi_pd = pd.read_csv(csv_path, encoding="utf-8")
    
    # 将CSV中的数据帧转换为CSI矩阵，这样得到数据矩阵维度就会变成 n x 3 x 3 x 114 x 2
    # Convert the data frame in the CSV to a CSI matrix, so that the dimension of the data matrix will become n x 3 x 3 x 114 x 2
    csi_matrix = csi_pd['csi_matrix']
    csi_matrix = [csi_matrix_str for csi_matrix_str in csi_matrix]  # 我们是以JSON字符串的形式存储的CSI矩阵，所以需要将其从JSON字符串转换为CSI矩阵
    csi_matrix = [eval(csi_matrix_str) for csi_matrix_str in csi_matrix]  # 将JSON字符串转换为CSI矩阵
    csi_matrix = np.array(csi_matrix)

    # 将CSI矩阵转换为numpy数组
    # Convert the CSI matrix to a numpy array
    csi_matrix = np.array(csi_matrix)

    # 将CSI矩阵的维度转化为 n x 3 x 3 x 114
    # Convert the dimension of the CSI matrix to n x 3 x 3 x 114
    csi_matrix = csi_matrix[:, :, :, 0] + 1j * csi_matrix[:, :, :, 1]

    return csi_matrix


# def preprocess_matrix(csi_matrix):
    
#     # combine the real and imaginary part of the CSI matrix to form a complex number
#     csi_matrix = csi_matrix[:, :, :, 0] + 1j * csi_matrix[:, :, :, 1]

#     # get the maximum value of the CSI matrix
#     max_csi = np.max(csi_matrix) + 1e-10

#     # get the minimum value of the CSI matrix
#     min_csi = np.min(csi_matrix)

#     # normalize the CSI matrix to [0, 1]
#     csi_matrix = csi_matrix / (max_csi - min_csi)

#     return csi_matrix


# def amplitude_matrix(csi_matrix):

#     # get the amplitude of the CSI matrix
#     csi_matrix = np.abs(csi_matrix)

#     return csi_matrix


# def db_matrix(csi_matrix):
#     # get the amplitude of the CSI matrix
#     csi_matrix_abs = np.abs(csi_matrix)

#     # add a small constant to avoid divide by zero error
#     csi_matrix_abs += 1e-10

#     # convert the amplitude to dB
#     csi_matrix_db = 20 * np.log10(csi_matrix_abs)

#     return csi_matrix_db


# def phase_matrix(csi_matrix):

#     # get the phase of the CSI matrix
#     csi_matrix = np.angle(csi_matrix)

#     return csi_matrix


# def padding_csi_matrix(csi_list, nrx = 3, ntx = 3, nsubcarrier = 56):

#     # temporary CSI list
#     csi_list_temp = []

#     # padding the csi data to the same size
#     for csi_data in csi_list:
        
#         # get the shape of the CSI matrix
#         nrx_temp, ntx_temp, nsubcarrier_temp, complex = csi_data.csi_matrix.shape

#         # if the number of rx antenna is not equal to the number of tx antenna
#         if nrx_temp != nrx or ntx_temp != ntx:

#             # get the number of antenna
#             nrx_max = max(nrx_temp, nrx)
#             ntx_max = max(ntx_temp, ntx)

#             # get the number of subcarrier
#             subcarrier_max = max(nsubcarrier_temp, nsubcarrier)

#             # create a new CSI matrix
#             csi_data_temp = np.zeros((nrx_max, ntx_max, subcarrier_max, complex), dtype=np.int32)

#             # copy the data from the original CSI matrix to the new CSI matrix
#             csi_data_temp[:nrx_temp, :ntx_temp, :nsubcarrier_temp, :] = csi_data.csi_matrix

#             # update the CSI data
#             csi_data.csi_matrix = csi_data_temp
        
#         # print out the shape of the CSI matrix
#         # print(csi_data.csi_matrix.shape)
        
#         # append the CSI data to the temporary CSI list
#         csi_list_temp.append(csi_data)

#     return csi_list_temp
