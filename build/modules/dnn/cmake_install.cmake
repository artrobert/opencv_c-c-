# Install script for directory: C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/dnn

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/artin/Desktop/build/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/x86/mingw/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/artin/Desktop/build/lib/libopencv_dnn320.dll.a")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "libs" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/x86/mingw/bin" TYPE SHARED_LIBRARY OPTIONAL FILES "C:/Users/artin/Desktop/build/bin/libopencv_dnn320.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/x86/mingw/bin/libopencv_dnn320.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/x86/mingw/bin/libopencv_dnn320.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/MinGW/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/x86/mingw/bin/libopencv_dnn320.dll")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2" TYPE FILE OPTIONAL FILES "C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/dnn/include/opencv2/dnn.hpp")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/dnn" TYPE FILE OPTIONAL FILES "C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/dnn/include/opencv2/dnn/all_layers.hpp")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/dnn" TYPE FILE OPTIONAL FILES "C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/dnn/include/opencv2/dnn/blob.hpp")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/dnn" TYPE FILE OPTIONAL FILES "C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/dnn/include/opencv2/dnn/blob.inl.hpp")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/dnn" TYPE FILE OPTIONAL FILES "C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/dnn/include/opencv2/dnn/dict.hpp")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/dnn" TYPE FILE OPTIONAL FILES "C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/dnn/include/opencv2/dnn/dnn.hpp")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/dnn" TYPE FILE OPTIONAL FILES "C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/dnn/include/opencv2/dnn/dnn.inl.hpp")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/dnn" TYPE FILE OPTIONAL FILES "C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/dnn/include/opencv2/dnn/layer.hpp")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/dnn" TYPE FILE OPTIONAL FILES "C:/Users/artin/Desktop/opencv_contrib-3.2.0/modules/dnn/include/opencv2/dnn/shape_utils.hpp")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "tests" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/x86/mingw/bin" TYPE EXECUTABLE FILES "C:/Users/artin/Desktop/build/bin/opencv_test_dnn.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/x86/mingw/bin/opencv_test_dnn.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/x86/mingw/bin/opencv_test_dnn.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/MinGW/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/x86/mingw/bin/opencv_test_dnn.exe")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "tests" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/x86/mingw/bin" TYPE EXECUTABLE FILES "C:/Users/artin/Desktop/build/bin/opencv_perf_dnn.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/x86/mingw/bin/opencv_perf_dnn.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/x86/mingw/bin/opencv_perf_dnn.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/MinGW/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/x86/mingw/bin/opencv_perf_dnn.exe")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/artin/Desktop/build/modules/dnn/3rdparty/protobuf/cmake_install.cmake")

endif()

