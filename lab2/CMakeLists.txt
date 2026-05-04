cmake_minimum_required(VERSION 3.16)
project(lab02_hough_pratos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_executable(lab02_hough_pratos
    src/main.cpp
    src/config.cpp
    src/plate_detector.cpp
    src/io_utils.cpp
)

add_executable(review_labels
    src/review_labels.cpp
)

target_include_directories(lab02_hough_pratos
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(review_labels
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(lab02_hough_pratos PRIVATE ${OpenCV_LIBS})
target_link_libraries(review_labels PRIVATE ${OpenCV_LIBS})
