# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv-3.2.0/modules/ml
# Build directory: C:/Users/artin/Desktop/build/modules/ml
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_ml "C:/Users/artin/Desktop/build/bin/opencv_test_ml.exe" "--gtest_output=xml:opencv_test_ml.xml")
set_tests_properties(opencv_test_ml PROPERTIES  LABELS "Main;opencv_ml;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
