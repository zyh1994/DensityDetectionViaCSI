cmake_minimum_required(VERSION 3.10)
project(pc_sampling)

# Set the CPP standard, we use C++14 here
set(CMAKE_CXX_STANDARD 14)

# Allow to include all necessary files
file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/csi/*"
        "${PROJECT_SOURCE_DIR}/cv/*"
        "${PROJECT_SOURCE_DIR}/network/*"
        "${PROJECT_SOURCE_DIR}/security/*"
        "${PROJECT_SOURCE_DIR}/storage/*"
        "${PROJECT_SOURCE_DIR}/Main.cpp"
        )

add_executable(pc_sampling ${all_SRCS})

# Get Ubuntu version using lsb_release command
execute_process(COMMAND lsb_release -rs
        OUTPUT_VARIABLE UBUNTU_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )

# Check the Ubuntu version
if(${UBUNTU_VERSION} MATCHES "16.04")
    # Ubuntu 16.04 specific configuration
    message(STATUS "Configuring for Ubuntu 16.04")

    # include opencv's header files
    include_directories(/usr/include/opencv)

    # link opencv's libraries
    target_link_libraries(pc_sampling
            opencv_calib3d
            opencv_contrib
            opencv_core
            opencv_features2d
            opencv_flann
            opencv_gpu
            opencv_highgui
            opencv_imgproc
            opencv_legacy
            opencv_ml
            opencv_objdetect
            opencv_ocl
            opencv_photo
            opencv_stitching
            opencv_superres
            opencv_ts
            opencv_video
            opencv_videostab
            )
elseif(${UBUNTU_VERSION} MATCHES "20.04")
    # Ubuntu 20.04 specific configuration
    message(STATUS "Configuring for Ubuntu 20.04")

    # include opencv's header files
    include_directories(/usr/include/opencv4)

    # link opencv's libraries
    target_link_libraries(pc_sampling
            opencv_stitching
            opencv_alphamat
            opencv_aruco
            opencv_barcode
            opencv_bgsegm
            opencv_bioinspired
            opencv_ccalib
            opencv_dnn_objdetect
            opencv_dnn_superres
            opencv_dpm
            opencv_face
            opencv_freetype
            opencv_fuzzy
            opencv_hdf
            opencv_hfs
            opencv_img_hash
            opencv_intensity_transform
            opencv_line_descriptor
            opencv_mcc
            opencv_quality
            opencv_rapid
            opencv_reg
            opencv_rgbd
            opencv_saliency
            opencv_shape
            opencv_stereo
            opencv_structured_light
            opencv_phase_unwrapping
            opencv_superres
            opencv_optflow
            opencv_surface_matching
            opencv_tracking
            opencv_highgui
            opencv_datasets
            opencv_text
            opencv_plot
            opencv_ml
            opencv_videostab
            opencv_videoio
            opencv_viz
            opencv_wechat_qrcode
            opencv_ximgproc
            opencv_video
            opencv_xobjdetect
            opencv_objdetect
            opencv_calib3d
            opencv_imgcodecs
            opencv_features2d
            opencv_dnn
            opencv_flann
            opencv_xphoto
            opencv_photo
            opencv_imgproc
            opencv_core
            )
elseif(APPLE)
    message(STATUS "Configuring for macOS")

    # Set the include directories and libraries obtained from pkg-config
    set(OpenCV_INCLUDE_DIRS "/usr/local/opt/opencv/include/opencv4")
    set(OpenCV_LIBRARIES "-L/usr/local/opt/opencv/lib -lopencv_gapi -lopencv_stitching -lopencv_alphamat -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_dnn_objdetect -lopencv_dnn_superres -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hfs -lopencv_img_hash -lopencv_intensity_transform -lopencv_line_descriptor -lopencv_mcc -lopencv_quality -lopencv_rapid -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_sfm -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_superres -lopencv_optflow -lopencv_surface_matching -lopencv_tracking -lopencv_highgui -lopencv_datasets -lopencv_text -lopencv_plot -lopencv_videostab -lopencv_videoio -lopencv_viz -lopencv_wechat_qrcode -lopencv_xfeatures2d -lopencv_shape -lopencv_ml -lopencv_ximgproc -lopencv_photo -lopencv_imgproc -lopencv_core")

    # Set the include directories and link the libraries
    include_directories(${OpenCV_INCLUDE_DIRS})
    target_link_libraries(pc_sampling ${OpenCV_LIBRARIES})

else()
    message(FATAL_ERROR "Unsupported Ubuntu version: ${UBUNTU_VERSION}")
endif()
