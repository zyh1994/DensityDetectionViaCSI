from Plotter import plot_csi, plot_single_csi


from ReadLogFile import read_log_file


sample_data = read_log_file('csi.dat')
signal_frame = sample_data[0]

plot_single_csi(signal_frame)

