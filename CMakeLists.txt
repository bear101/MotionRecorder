cmake_minimum_required(VERSION 3.1)

project(MotionRecorder)

# include(ExternalProject)

# ExternalProject_Add(ffmpeg
#  GIT_REPOSITORY https://github.com/FFmpeg/FFmpeg
#  GIT_TAG n3.3.7
#  BUILD_IN_SOURCE 1
#  CONFIGURE_COMMAND ./configure --prefix=$ENV{PWD}
#  BUILD_COMMAND make -j2 COMMAND echo done)

# ExternalProject_Add(opencv
#  GIT_REPOSITORY https://github.com/opencv/opencv
#  GIT_TAG 2.4.13.6
#  #  DEPENDS ffmpeg
#  CONFIGURE_COMMAND ./configure --help
#  BUILD_COMMAND true COMMAND echo done)

# paths 
include_directories (/Users/bdr/libraries/opencv/include)
                     
link_directories (/Users/bdr/libraries/opencv/lib)

# SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
set (OPENCV_LIBRARIES opencv_core opencv_imgproc opencv_imgcodecs opencv_videoio)

add_executable (motion main.cpp)
target_link_libraries (motion ${OPENCV_LIBRARIES})

# add_dependencies(motion opencv)
# SET(CMAKE_BUILD_TYPE Release)

