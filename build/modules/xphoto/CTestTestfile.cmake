# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/xphoto
# Build directory: C:/Users/artin/Desktop/build/modules/xphoto
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_xphoto "C:/Users/artin/Desktop/build/bin/opencv_test_xphoto.exe" "--gtest_output=xml:opencv_test_xphoto.xml")
set_tests_properties(opencv_test_xphoto PROPERTIES  LABELS "Extra;opencv_xphoto;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
add_test(opencv_perf_xphoto "C:/Users/artin/Desktop/build/bin/opencv_perf_xphoto.exe" "--gtest_output=xml:opencv_perf_xphoto.xml")
set_tests_properties(opencv_perf_xphoto PROPERTIES  LABELS "Extra;opencv_xphoto;Performance" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/performance")
add_test(opencv_sanity_xphoto "C:/Users/artin/Desktop/build/bin/opencv_perf_xphoto.exe" "--gtest_output=xml:opencv_perf_xphoto.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
set_tests_properties(opencv_sanity_xphoto PROPERTIES  LABELS "Extra;opencv_xphoto;Sanity" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/sanity")
