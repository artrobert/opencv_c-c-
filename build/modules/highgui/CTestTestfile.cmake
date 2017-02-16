# CMake generated Testfile for 
# Source directory: C:/Users/rartin/Desktop/opencv/sources/modules/highgui
# Build directory: C:/Users/rartin/Desktop/build/modules/highgui
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_highgui "C:/Users/rartin/Desktop/build/bin/opencv_test_highgui.exe" "--gtest_output=xml:opencv_test_highgui.xml")
set_tests_properties(opencv_test_highgui PROPERTIES  LABELS "Main;opencv_highgui;Accuracy" WORKING_DIRECTORY "C:/Users/rartin/Desktop/build/test-reports/accuracy")
