# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/tracking
# Build directory: C:/Users/artin/Desktop/build/modules/tracking
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_tracking "C:/Users/artin/Desktop/build/bin/opencv_test_tracking.exe" "--gtest_output=xml:opencv_test_tracking.xml")
set_tests_properties(opencv_test_tracking PROPERTIES  LABELS "Extra;opencv_tracking;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
add_test(opencv_perf_tracking "C:/Users/artin/Desktop/build/bin/opencv_perf_tracking.exe" "--gtest_output=xml:opencv_perf_tracking.xml")
set_tests_properties(opencv_perf_tracking PROPERTIES  LABELS "Extra;opencv_tracking;Performance" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/performance")
add_test(opencv_sanity_tracking "C:/Users/artin/Desktop/build/bin/opencv_perf_tracking.exe" "--gtest_output=xml:opencv_perf_tracking.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
set_tests_properties(opencv_sanity_tracking PROPERTIES  LABELS "Extra;opencv_tracking;Sanity" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/sanity")
