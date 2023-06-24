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
