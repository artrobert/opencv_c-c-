cmake_minimum_required(VERSION 3.6)
project(EdgeDetection)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Where to find CMake modules and OpenCV
set(OpenCV_DIR "D:\\Facultate\\c++Project\\opencv_c-c-\\build\\install")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCE_FILES
        main.cpp
        IPM.cpp
        IPM.h
        ImageBasicOperations.cpp
        ImageBasicOperations.h
        FeatureDetection.cpp
        FeatureDetection.h
        KMeansImplementation.cpp
        KMeansImplementation.h
        EdgeDetecting.cpp
        EdgeDetecting.h
        backgroundSubtract.cpp
        backgroundSubtract.h
        ImageDB.cpp
        ImageDB.h
        models/PieceContour.cpp
        models/PieceContour.h
        models/PieceModel.cpp
        models/PieceModel.h
        models/PieceType.h
        models/ChessSquare.cpp
        models/ChessSquare.h models/ChessSquareMatrix.cpp models/ChessSquareMatrix.h)

add_executable(test_cv ${SOURCE_FILES})

#add libs you need
set(OpenCV_LIBS
        opencv_core
        opencv_imgproc
        opencv_highgui
        opencv_imgcodecs
        opencv_features2d
        opencv_videoio
        opencv_video
        opencv_shape)

#linking
target_link_libraries(test_cv ${OpenCV_LIBS})

#
#set(CMAKE_CXX_STANDARD 11)
#
#set(SOURCE_FILES main.cpp)
#add_executable(EdgeDetection ${SOURCE_FILES})

