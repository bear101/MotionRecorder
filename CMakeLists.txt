cmake_minimum_required(VERSION 3.1)

project(MotionRecorder)

include(ExternalProject)

ExternalProject_Add(ffmpeg
  GIT_REPOSITORY https://github.com/FFmpeg/FFmpeg
  GIT_TAG n4.0
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ./configure --prefix=$ENV{PWD}/libs/ffmpeg --enable-shared --disable-doc
  BUILD_COMMAND make -j2 install
  COMMAND echo done)

ExternalProject_Add(opencv
  GIT_REPOSITORY https://github.com/opencv/opencv
  GIT_TAG 3.4.1
  #  DEPENDS ffmpeg
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=$ENV{PWD}/libs/opencv -DCMAKE_C_FLAGS="-I$ENV{PWD}/libs/ffmpeg/include" -DCMAKE_CXX_FLAGS="-I$ENV{PWD}/libs/ffmpeg/include" -DCMAKE_SHARED_LINKER_FLAGS="-L$ENV{PWD}/libs/ffmpeg/lib"
  BUILD_COMMAND make -j2 install
  COMMAND echo done)

# paths 
include_directories ($ENV{PWD}/libs/opencv/include)
                     
link_directories ($ENV{PWD}/libs/opencv/lib)

# SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
set (OPENCV_LIBRARIES opencv_core opencv_imgproc opencv_imgcodecs opencv_videoio)

add_executable (motion main.cpp)
target_link_libraries (motion ${OPENCV_LIBRARIES})

# add_dependencies(motion opencv)
# SET(CMAKE_BUILD_TYPE Release)

