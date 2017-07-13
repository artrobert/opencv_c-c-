# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv-3.2.0/modules/shape
# Build directory: C:/Users/artin/Desktop/build/modules/shape
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_shape "C:/Users/artin/Desktop/build/bin/opencv_test_shape.exe" "--gtest_output=xml:opencv_test_shape.xml")
set_tests_properties(opencv_test_shape PROPERTIES  LABELS "Main;opencv_shape;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
