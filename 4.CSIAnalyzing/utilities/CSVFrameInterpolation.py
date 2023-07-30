import copy

from csi.CSVDataLoader import CSIDataFromCSV


def csi_frame_with_contributed_ratio(csi_frame: CSIDataFromCSV, contribute_ratio: float):

    # Deep copy the CSI frame
    csi_frame_copied = copy.deepcopy(csi_frame)

    # Calculate the CSI matrix with the contributed ratio
    csi_frame_copied.csi_matrix = csi_frame_copied.csi_matrix * contribute_ratio

    # Calculate the RSSI with the contributed ratio
    csi_frame_copied.rssi_antenna_0 = csi_frame_copied.rssi_antenna_0 * contribute_ratio
    csi_frame_copied.rssi_antenna_1 = csi_frame_copied.rssi_antenna_1 * contribute_ratio
    csi_frame_copied.rssi_antenna_2 = csi_frame_copied.rssi_antenna_2 * contribute_ratio

    # Return the CSI frame with the contributed ratio
    return csi_frame_copied


def csi_frames_merged(frame1: CSIDataFromCSV, frame2: CSIDataFromCSV):

    """
    timestamp: 时间戳
    """

    # Check if the parameters excepting the CSI matrix are the same
    if frame1.csi_length != frame2.csi_length:
        raise ValueError("The csi length of the two frames are not the same")

    if frame1.channel_number != frame2.channel_number:
        raise ValueError("The channel number of the two frames are not the same")

    if frame1.buffer_length != frame2.buffer_length:
        raise ValueError("The buffer length of the two frames are not the same")

    if frame1.payload_length != frame2.payload_length:
        raise ValueError("The payload length of the two frames are not the same")

    if frame1.physical_error != frame2.physical_error:
        raise ValueError("The physical error of the two frames are not the same")

    if frame1.noise_level != frame2.noise_level:
        raise ValueError("The noise level of the two frames are not the same")

    if frame1.transmission_rate != frame2.transmission_rate:
        raise ValueError("The transmission rate of the two frames are not the same")

    if frame1.channel_bandwidth != frame2.channel_bandwidth:
        raise ValueError("The channel bandwidth of the two frames are not the same")

    if frame1.number_of_tones != frame2.number_of_tones:
        raise ValueError("The number of tones of the two frames are not the same")

    if frame1.receiver_antennas != frame2.receiver_antennas:
        raise ValueError("The receiver antennas of the two frames are not the same")

    if frame1.transmitter_antennas != frame2.transmitter_antennas:
        raise ValueError("The transmitter antennas of the two frames are not the same")

    if frame1.received_signal_strength != frame2.received_signal_strength:
        raise ValueError("The received signal strength of the two frames are not the same")

    # Calculate the average of the RSSI
    if frame1.rssi_antenna_0 != frame2.rssi_antenna_0:
        frame1.rssi_antenna_0 = int((frame2.rssi_antenna_0 + frame1.rssi_antenna_0))

    if frame1.rssi_antenna_1 != frame2.rssi_antenna_1:
        frame1.rssi_antenna_1 = int((frame2.rssi_antenna_1 + frame1.rssi_antenna_1))

    if frame1.rssi_antenna_2 != frame2.rssi_antenna_2:
        frame1.rssi_antenna_2 = int((frame2.rssi_antenna_2 + frame1.rssi_antenna_2))

    # Merge the CSI matrix
    frame1.csi_matrix = frame1.csi_matrix + frame2.csi_matrix
    frame1.csi_matrix = frame1.csi_matrix.astype(int)

    return frame1


def linear_interpolation(csi_frame_1, csi_frame_2, contribute_ratio):

    # Calculate the interpolated data with the linear interpolation
    csi_frame_1 = csi_frame_with_contributed_ratio(csi_frame_1, 1 - contribute_ratio)
    csi_frame_2 = csi_frame_with_contributed_ratio(csi_frame_2, contribute_ratio)

    # Merge the two CSI frames
    csi_frame_merged = csi_frames_merged(csi_frame_1, csi_frame_2)

    # Return the merged CSI frame
    return csi_frame_merged


def data_interpolate(data_frames, theoretical_data_volume, time_duration_in_seconds):

    # Get the number of frames
    num_of_frames = len(data_frames)

    # Get the interval between two frames in theoretical
    time_duration_in_milliseconds = time_duration_in_seconds * 1000
    sampling_interval = time_duration_in_milliseconds / theoretical_data_volume

    # Create an empty list for the padded data
    padded_data_frames = []

    # Interpolate the CSI data between two frames
    for idx in range(num_of_frames):
        if idx == num_of_frames - 1:
            break

        # If the frame is the first frame, append it to the padded data
        if idx == 0:
            padded_data_frames.append(data_frames[idx])
            continue

        # Get the current frame and the next frame
        current_frame = data_frames[idx]
        next_frame = data_frames[idx + 1]

        # Use the linear interpolation to interpolate the CSI data
        duration_between_frames = next_frame.timestamp - current_frame.timestamp

        # Get the ratio between the duration between frames and the sampling interval
        contribute_ratio = float(duration_between_frames / sampling_interval)

        # Get the interpolated CSI frame
        interpolated_frame = linear_interpolation(current_frame, next_frame, contribute_ratio)


    return padded_data_frames