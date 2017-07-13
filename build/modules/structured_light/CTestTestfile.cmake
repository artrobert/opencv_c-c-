# CMake generated Testfile for 
# Source directory: C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/structured_light
# Build directory: C:/Users/artin/Desktop/build/modules/structured_light
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_structured_light "C:/Users/artin/Desktop/build/bin/opencv_test_structured_light.exe" "--gtest_output=xml:opencv_test_structured_light.xml")
set_tests_properties(opencv_test_structured_light PROPERTIES  LABELS "Extra;opencv_structured_light;Accuracy" WORKING_DIRECTORY "C:/Users/artin/Desktop/build/test-reports/accuracy")
