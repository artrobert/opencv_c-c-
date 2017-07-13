# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/ximgproc
# Build directory: C:/Users/artin/Desktop/build/modules/ximgproc
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_ximgproc "C:/Users/artin/Desktop/build/bin/opencv_test_ximgproc.exe" "--gtest_output=xml:opencv_test_ximgproc.xml")
set_tests_properties(opencv_test_ximgproc PROPERTIES  LABELS "Extra;opencv_ximgproc;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
add_test(opencv_perf_ximgproc "C:/Users/artin/Desktop/build/bin/opencv_perf_ximgproc.exe" "--gtest_output=xml:opencv_perf_ximgproc.xml")
set_tests_properties(opencv_perf_ximgproc PROPERTIES  LABELS "Extra;opencv_ximgproc;Performance" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/performance")
add_test(opencv_sanity_ximgproc "C:/Users/artin/Desktop/build/bin/opencv_perf_ximgproc.exe" "--gtest_output=xml:opencv_perf_ximgproc.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
set_tests_properties(opencv_sanity_ximgproc PROPERTIES  LABELS "Extra;opencv_ximgproc;Sanity" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/sanity")
