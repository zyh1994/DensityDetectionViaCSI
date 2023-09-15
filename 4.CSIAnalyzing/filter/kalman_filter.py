import numpy as np


def kalman_filter(signal, 
                  initial_state_estimate = np.array([0.0]),      # 初始状态估计值
                  initial_estimate_covariance = np.eye(1),       # 初始状态估计的协方差矩阵
                  process_noise_covariance = np.diag([0.01]),    # 过程噪音协方差矩阵
                  measurement_noise_covariance = 0.04):          # 测量噪音的方差
    
    """
    卡尔曼滤波器

    @param signal: 信号
    @param initial_state_estimate: 初始状态估计值
    @param initial_estimate_covariance: 初始状态估计的协方差矩阵
    @param process_noise_covariance: 过程噪音协方差矩阵
    @param measurement_noise_covariance: 测量噪音的方差

    @return: 滤波后的信号估计值
    """

    state_estimate = initial_state_estimate
    estimate_covariance = initial_estimate_covariance
    filtered_signal_estimates = []

    # 定义卡尔曼滤波器的状态转移矩阵和测量模型
    # Define the state transition matrix and measurement matrix of Kalman filter
    state_transition_matrix = np.array([[1.0]])
    measurement_matrix = np.array([[1.0]])

    for measurement in signal:
        # 预测步骤（时间更新）
        # Prediction step (time update)
        state_estimate = np.dot(state_transition_matrix, state_estimate)
        estimate_covariance = np.dot(np.dot(state_transition_matrix, estimate_covariance), state_transition_matrix.T) + process_noise_covariance

        # 更新步骤（测量更新）
        # Update step (measurement update)
        innovation = measurement - np.dot(measurement_matrix, state_estimate)
        innovation_covariance = np.dot(np.dot(measurement_matrix, estimate_covariance), measurement_matrix.T) + measurement_noise_covariance
        kalman_gain = np.dot(np.dot(estimate_covariance, measurement_matrix.T), np.linalg.inv(innovation_covariance))

        state_estimate = state_estimate + np.dot(kalman_gain, innovation)
        estimate_covariance = estimate_covariance - np.dot(np.dot(kalman_gain, measurement_matrix), estimate_covariance)

        # 将滤波后的信号估计值保存到列表中
        # Save the filtered signal estimate to the list
        filtered_signal_estimates.append(state_estimate[0])

    return np.array(filtered_signal_estimates)
