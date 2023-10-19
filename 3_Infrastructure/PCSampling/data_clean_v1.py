import os
import pandas as pd
import numpy as np
import json
import sys



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
    # csi_matrix = [csi[:, :, :, 0] + 1j * csi[:, :, :, 1] for csi in csi_matrix]  # 将CSI矩阵转换为复数矩阵

    # 将转换后的CSI矩阵，重新赋值给csi_pd
    # Assign the converted CSI matrix to csi_pd
    csi_pd['csi_matrix_json'] = csi_matrix

    return csi_pd



def assess_data_cleanliness(csi_pd):
    
    # 这里，我们要开始检查数据维度的统一性
    # Here, we are going to start checking the uniformity of data dimensions
    csi_matrix = csi_pd['csi_matrix_json']

    # 首先，我们统计数据中出现的维度，并对其进行排序
    # First, we count the dimensions that appear in the data and sort them
    dimension_set = set()

    for csi in csi_matrix:
        dimension_set.add(csi.shape)

    dimension_list = list(dimension_set)
    dimension_list.sort()

    # 然后，我们统计每个维度出现的次数
    # Then, we count the number of occurrences of each dimension
    dimension_count = [0 for i in range(len(dimension_list))]
    
    for csi in csi_matrix:
        dimension_count[dimension_list.index(csi.shape)] += 1
    
    total_count = sum(dimension_count)

    # 最后，我们输出统计结果
    # Finally, we output the statistical results
    print("The number of different dimensions in the data is: ", len(dimension_list))
    print("The dimensions that appear in the data are: ", dimension_list)
    print("The number of occurrences of each dimension is: ", dimension_count)
    print("The percentage of each dimension is: ", [str(int(count / total_count * 10000) / 100) + "%" for count in dimension_count])

    return dimension_list, dimension_count



def data_cleaning(csi_pd, dimension_list, dimension_count):

    # 找出dimension_count里最大的一项，并找出其对应的dimension_list的索引
    # Find the largest item in dimension_count and find the index of its corresponding dimension_list
    max_dimension_index = dimension_count.index(max(dimension_count))

    # 找出dimension_list里最大的一项，并找出其对应的dimension_count的索引
    # Find the largest item in dimension_list and find the index of its corresponding dimension_count
    max_dimension = dimension_list[max_dimension_index]

    # 保留最大的dimension_list里最大的一项，其余的全部删除
    # Keep the largest item in the largest dimension_list, and delete the rest
    csi_pd = csi_pd[csi_pd['csi_matrix_json'].map(lambda csi: csi.shape == max_dimension)]

    return csi_pd


def clean_csv(csv_path: str):
    # 测试用数据
    csi_pd = load_csi_matrix_from_file(csv_path)

    # 检查数据维度的统一性
    # Check the uniformity of data dimensions
    dimension_list, dimension_count = assess_data_cleanliness(csi_pd)

    # 数据清洗
    # Data cleaning
    csi_pd = data_cleaning(csi_pd, dimension_list, dimension_count)

    # 检查数据维度的统一性
    # Check the uniformity of data dimensions
    print('---------------------------After data cleaning---------------------------')
    dimension_list, dimension_count = assess_data_cleanliness(csi_pd)
    print('\n')

    # 另存名字
    # Another saving name
    csv_path = csv_path[:-4] + '_cleaned.csv'

    # 删除csi_matrix_json列
    # Delete the csi_matrix_json column
    del csi_pd['csi_matrix_json']

    # 将清洗后的数据保存到csv文件中
    # Save the cleaned data to a csv file
    csi_pd.to_csv(csv_path, sep='\t', encoding='utf-8', index=False)


def do_cleaning(csv_path: str):
    # Scan the directory and get all the csv files
    csv_path_list = []
    for root, dirs, files in os.walk(csv_path):
        for file in files:
            if file.endswith('csi.csv'):
                csv_path_list.append(os.path.join(root, file))

    # Clean the data    
    for csv_path in csv_path_list:
        clean_csv(csv_path)


if __name__ == "__main__":

    # print out the usage
    if len(sys.argv) == 2 and sys.argv[1] == '-h': 
        print("Usage: python data_clean_v1.py <root_folder>")
    
    # Move the files to a folder
    do_cleaning(sys.argv[1])
