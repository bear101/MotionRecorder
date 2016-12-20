cmake_minimum_required(VERSION 2.4)

PROJECT(MotionRecorder)
# paths 
INCLUDE_DIRECTORIES(/home/bdr/libraries/opencv-2.4/include)
LINK_DIRECTORIES(/home/bdr/libraries/opencv-2.4/lib)
# SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
SET(OpenCV_LIBRARIES opencv_core opencv_highgui opencv_imgproc opencv_video)

ADD_EXECUTABLE(motion main.cpp)
TARGET_LINK_LIBRARIES(motion ${OpenCV_LIBRARIES})

# SET(CMAKE_BUILD_TYPE Release)

