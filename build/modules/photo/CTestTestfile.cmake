# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv-3.2.0/modules/photo
# Build directory: C:/Users/artin/Desktop/build/modules/photo
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_photo "C:/Users/artin/Desktop/build/bin/opencv_test_photo.exe" "--gtest_output=xml:opencv_test_photo.xml")
set_tests_properties(opencv_test_photo PROPERTIES  LABELS "Main;opencv_photo;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
add_test(opencv_perf_photo "C:/Users/artin/Desktop/build/bin/opencv_perf_photo.exe" "--gtest_output=xml:opencv_perf_photo.xml")
set_tests_properties(opencv_perf_photo PROPERTIES  LABELS "Main;opencv_photo;Performance" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/performance")
add_test(opencv_sanity_photo "C:/Users/artin/Desktop/build/bin/opencv_perf_photo.exe" "--gtest_output=xml:opencv_perf_photo.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
set_tests_properties(opencv_sanity_photo PROPERTIES  LABELS "Main;opencv_photo;Sanity" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/sanity")
