# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/dnn
# Build directory: C:/Users/artin/Desktop/build/modules/dnn
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_dnn "C:/Users/artin/Desktop/build/bin/opencv_test_dnn.exe" "--gtest_output=xml:opencv_test_dnn.xml")
set_tests_properties(opencv_test_dnn PROPERTIES  LABELS "Extra;opencv_dnn;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
add_test(opencv_perf_dnn "C:/Users/artin/Desktop/build/bin/opencv_perf_dnn.exe" "--gtest_output=xml:opencv_perf_dnn.xml")
set_tests_properties(opencv_perf_dnn PROPERTIES  LABELS "Extra;opencv_dnn;Performance" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/performance")
add_test(opencv_sanity_dnn "C:/Users/artin/Desktop/build/bin/opencv_perf_dnn.exe" "--gtest_output=xml:opencv_perf_dnn.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
set_tests_properties(opencv_sanity_dnn PROPERTIES  LABELS "Extra;opencv_dnn;Sanity" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/sanity")
subdirs("3rdparty/protobuf")
