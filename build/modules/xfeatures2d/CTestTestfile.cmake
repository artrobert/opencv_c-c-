# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/xfeatures2d
# Build directory: C:/Users/artin/Desktop/build/modules/xfeatures2d
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_xfeatures2d "C:/Users/artin/Desktop/build/bin/opencv_test_xfeatures2d.exe" "--gtest_output=xml:opencv_test_xfeatures2d.xml")
set_tests_properties(opencv_test_xfeatures2d PROPERTIES  LABELS "Extra;opencv_xfeatures2d;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
add_test(opencv_perf_xfeatures2d "C:/Users/artin/Desktop/build/bin/opencv_perf_xfeatures2d.exe" "--gtest_output=xml:opencv_perf_xfeatures2d.xml")
set_tests_properties(opencv_perf_xfeatures2d PROPERTIES  LABELS "Extra;opencv_xfeatures2d;Performance" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/performance")
add_test(opencv_sanity_xfeatures2d "C:/Users/artin/Desktop/build/bin/opencv_perf_xfeatures2d.exe" "--gtest_output=xml:opencv_perf_xfeatures2d.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
set_tests_properties(opencv_sanity_xfeatures2d PROPERTIES  LABELS "Extra;opencv_xfeatures2d;Sanity" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/sanity")
