import numpy as np

from plot.MatrixPreprocessor import db_matrix, preprocess_matrix


def gen_heatmap_matrix(csi_data: list):

    # matrix list for concatenation
    matrix_list = []

    # get the csi matrix from the csi data
    for csi_matrix in csi_data:

        # preprocess the csi matrix
        csi_matrix = preprocess_matrix(csi_matrix.csi_matrix)

        # calculate the db of the csi matrix
        csi_matrix = db_matrix(csi_matrix)

        # reshape the csi matrix into one dimension
        csi_matrix = csi_matrix.reshape(-1)

        # add the csi matrix to the matrix list
        matrix_list.append(csi_matrix)

    # reshape the matrices to have a second dimension
    matrix_list = [matrix[:, np.newaxis] for matrix in matrix_list]

    # concatenate the matrix list along axis 1
    matrix = np.concatenate(matrix_list, axis=1)

    # normalize the matrix to [0, 1]
    matrix = matrix / (np.max(matrix) - np.min(matrix))

    return matrix