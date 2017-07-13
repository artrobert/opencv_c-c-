# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/reg
# Build directory: C:/Users/artin/Desktop/build/modules/reg
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_reg "C:/Users/artin/Desktop/build/bin/opencv_test_reg.exe" "--gtest_output=xml:opencv_test_reg.xml")
set_tests_properties(opencv_test_reg PROPERTIES  LABELS "Extra;opencv_reg;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
add_test(opencv_perf_reg "C:/Users/artin/Desktop/build/bin/opencv_perf_reg.exe" "--gtest_output=xml:opencv_perf_reg.xml")
set_tests_properties(opencv_perf_reg PROPERTIES  LABELS "Extra;opencv_reg;Performance" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/performance")
add_test(opencv_sanity_reg "C:/Users/artin/Desktop/build/bin/opencv_perf_reg.exe" "--gtest_output=xml:opencv_perf_reg.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
set_tests_properties(opencv_sanity_reg PROPERTIES  LABELS "Extra;opencv_reg;Sanity" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/sanity")
