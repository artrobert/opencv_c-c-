# CMake generated Testfile for 
# Source directory: C:/Users/rartin/Desktop/opencv/sources/modules/flann
# Build directory: C:/Users/rartin/Desktop/build/modules/flann
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_flann "C:/Users/rartin/Desktop/build/bin/opencv_test_flann.exe" "--gtest_output=xml:opencv_test_flann.xml")
set_tests_properties(opencv_test_flann PROPERTIES  LABELS "Main;opencv_flann;Accuracy" WORKING_DIRECTORY "C:/Users/rartin/Desktop/build/test-reports/accuracy")
