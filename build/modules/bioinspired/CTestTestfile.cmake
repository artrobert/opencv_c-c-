# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/bioinspired
# Build directory: C:/Users/artin/Desktop/build/modules/bioinspired
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_bioinspired "C:/Users/artin/Desktop/build/bin/opencv_test_bioinspired.exe" "--gtest_output=xml:opencv_test_bioinspired.xml")
set_tests_properties(opencv_test_bioinspired PROPERTIES  LABELS "Extra;opencv_bioinspired;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
add_test(opencv_perf_bioinspired "C:/Users/artin/Desktop/build/bin/opencv_perf_bioinspired.exe" "--gtest_output=xml:opencv_perf_bioinspired.xml")
set_tests_properties(opencv_perf_bioinspired PROPERTIES  LABELS "Extra;opencv_bioinspired;Performance" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/performance")
add_test(opencv_sanity_bioinspired "C:/Users/artin/Desktop/build/bin/opencv_perf_bioinspired.exe" "--gtest_output=xml:opencv_perf_bioinspired.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
set_tests_properties(opencv_sanity_bioinspired PROPERTIES  LABELS "Extra;opencv_bioinspired;Sanity" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/sanity")
