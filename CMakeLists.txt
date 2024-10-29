cmake_minimum_required(VERSION 3.1)

project(MotionRecorder)

include(ExternalProject)

if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
  set (CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set (OPENCV_LIBRARIES opencv_core opencv_imgproc opencv_imgcodecs opencv_videoio opencv_video)

ExternalProject_Add (opencv
  GIT_REPOSITORY    https://github.com/opencv/opencv
  GIT_TAG           4.10.0
  INSTALL_DIR       $ENV{PWD}/libs/opencv
  CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF
  BUILD_COMMAND     ${CMAKE_COMMAND} --build . --target install
  BUILD_BYPRODUCTS  ${OPENCV_LIBRARIES})

include_directories ($ENV{PWD}/libs/opencv/include/opencv4)
                     
link_directories ($ENV{PWD}/libs/opencv/lib)

add_executable (motion main.cpp)
target_link_libraries (motion ${OPENCV_LIBRARIES})

