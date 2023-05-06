cmake_minimum_required(VERSION 3.10)
project(ping)

# Set the CPP standard, we use C++14 here
set(CMAKE_CXX_STANDARD 14)

# Allow to include all necessary files
file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/src/*.h"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
        )

add_executable(ping ${all_SRCS} src/main.cpp src/main.cpp)