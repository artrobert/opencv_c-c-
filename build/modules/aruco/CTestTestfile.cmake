# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/aruco
# Build directory: C:/Users/artin/Desktop/build/modules/aruco
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_aruco "C:/Users/artin/Desktop/build/bin/opencv_test_aruco.exe" "--gtest_output=xml:opencv_test_aruco.xml")
set_tests_properties(opencv_test_aruco PROPERTIES  LABELS "Extra;opencv_aruco;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
