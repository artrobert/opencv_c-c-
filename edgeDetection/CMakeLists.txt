cmake_minimum_required(VERSION 3.6)
project(EdgeDetection)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Where to find CMake modules and OpenCV
set(OpenCV_DIR "D:\\Facultate\\c++Project\\opencv_c-c-\\build\\install")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(test_cv main.cpp IPM.cpp IPM.h imagePreparation.cpp imagePreparation.h featureDetection.cpp featureDetection.h)

#add libs you need
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs opencv_features2d)

#linking
target_link_libraries(test_cv ${OpenCV_LIBS})

#
#set(CMAKE_CXX_STANDARD 11)
#
#set(SOURCE_FILES main.cpp)
#add_executable(EdgeDetection ${SOURCE_FILES})

