# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv-3.2.0/modules/imgproc
# Build directory: C:/Users/artin/Desktop/build/modules/imgproc
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_imgproc "C:/Users/artin/Desktop/build/bin/opencv_test_imgproc.exe" "--gtest_output=xml:opencv_test_imgproc.xml")
set_tests_properties(opencv_test_imgproc PROPERTIES  LABELS "Main;opencv_imgproc;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
add_test(opencv_perf_imgproc "C:/Users/artin/Desktop/build/bin/opencv_perf_imgproc.exe" "--gtest_output=xml:opencv_perf_imgproc.xml")
set_tests_properties(opencv_perf_imgproc PROPERTIES  LABELS "Main;opencv_imgproc;Performance" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/performance")
add_test(opencv_sanity_imgproc "C:/Users/artin/Desktop/build/bin/opencv_perf_imgproc.exe" "--gtest_output=xml:opencv_perf_imgproc.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
set_tests_properties(opencv_sanity_imgproc PROPERTIES  LABELS "Main;opencv_imgproc;Sanity" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/sanity")
