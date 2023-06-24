cmake_minimum_required(VERSION 3.10)
project(pc_recorder)

# Set the CPP standard, we use C++14 here
set(CMAKE_CXX_STANDARD 14)

# Allow to include all necessary files
file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/*.h"
        "${PROJECT_SOURCE_DIR}/*.cpp"
        "${PROJECT_SOURCE_DIR}/*.c"
        )

add_executable(pc_recorder ${all_SRCS})

# include opencv's header files
# -I/usr/include/opencv4
include_directories(/usr/include/opencv4)

# link opencv's libraries
# -lopencv_stitching -lopencv_alphamat -lopencv_aruco -lopencv_barcode -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_dnn_objdetect -lopencv_dnn_superres -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hdf -lopencv_hfs -lopencv_img_hash -lopencv_intensity_transform -lopencv_line_descriptor -lopencv_mcc -lopencv_quality -lopencv_rapid -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_shape -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_superres -lopencv_optflow -lopencv_surface_matching -lopencv_tracking -lopencv_highgui -lopencv_datasets -lopencv_text -lopencv_plot -lopencv_ml -lopencv_videostab -lopencv_videoio -lopencv_viz -lopencv_wechat_qrcode -lopencv_ximgproc -lopencv_video -lopencv_xobjdetect -lopencv_objdetect -lopencv_calib3d -lopencv_imgcodecs -lopencv_features2d -lopencv_dnn -lopencv_flann -lopencv_xphoto -lopencv_photo -lopencv_imgproc -lopencv_core
target_link_libraries(pc_recorder opencv_stitching opencv_alphamat opencv_aruco opencv_barcode opencv_bgsegm
        opencv_bioinspired opencv_ccalib opencv_dnn_objdetect opencv_dnn_superres opencv_dpm opencv_face
        opencv_freetype opencv_fuzzy opencv_hdf opencv_hfs opencv_img_hash opencv_intensity_transform
        opencv_line_descriptor opencv_mcc opencv_quality opencv_rapid opencv_reg opencv_rgbd
        opencv_saliency opencv_shape opencv_stereo opencv_structured_light opencv_phase_unwrapping
        opencv_superres opencv_optflow opencv_surface_matching opencv_tracking opencv_highgui opencv_datasets
        opencv_text opencv_plot opencv_ml opencv_videostab opencv_videoio opencv_viz opencv_wechat_qrcode
        opencv_ximgproc opencv_video opencv_xobjdetect opencv_objdetect opencv_calib3d opencv_imgcodecs
        opencv_features2d opencv_dnn opencv_flann opencv_xphoto opencv_photo opencv_imgproc opencv_core)