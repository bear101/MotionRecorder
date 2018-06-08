cmake_minimum_required(VERSION 3.1)

project(MotionRecorder)

include(ExternalProject)

if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
  set (CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# paths 
include_directories ($ENV{PWD}/libs/opencv/include)
                     
link_directories ($ENV{PWD}/libs/opencv/lib)

set (OPENCV_LIBRARIES opencv_core opencv_imgproc opencv_imgcodecs opencv_videoio opencv_video)

add_executable (motion main.cpp)
target_link_libraries (motion ${OPENCV_LIBRARIES})

externalproject_add (ffmpeg
  GIT_REPOSITORY https://github.com/FFmpeg/FFmpeg
  GIT_TAG n4.0
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ./configure --prefix=$ENV{PWD}/libs/ffmpeg --enable-shared --disable-doc
  BUILD_COMMAND make -j2 install
  COMMAND echo done)

message ("To build using custom FFmpeg set the following env:
  - export LD_LIBRARY_PATH=$PWD/libs/ffmpeg/lib
  - export PKG_CONFIG_PATH=$PWD/libs/ffmpeg/lib/pkgconfig
  - export PKG_CONFIG_LIBDIR=$PWD/libs/ffmpeg/lib")

externalproject_add (opencv
  GIT_REPOSITORY https://github.com/opencv/opencv
  GIT_TAG 3.4.1
  #  DEPENDS ffmpeg
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=$ENV{PWD}/libs/opencv
  BUILD_COMMAND make -j2 install
  COMMAND echo done)
