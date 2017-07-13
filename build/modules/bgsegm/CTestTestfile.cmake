# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/bgsegm
# Build directory: C:/Users/artin/Desktop/build/modules/bgsegm
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_bgsegm "C:/Users/artin/Desktop/build/bin/opencv_test_bgsegm.exe" "--gtest_output=xml:opencv_test_bgsegm.xml")
set_tests_properties(opencv_test_bgsegm PROPERTIES  LABELS "Extra;opencv_bgsegm;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
