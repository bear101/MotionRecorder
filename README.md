# MotionRecorder
Motion detector for surveillance camera based on OpenCV and FFMPEG.

Inspired by https://github.com/cedricve/motion-detection

# Building

Use Debian 12 and install the following dependencies:

`apt install   autoconf   automake   build-essential   cmake   libtool   meson   ninja-build   pkg-config   wget   yasm   zlib1g-dev   nasm   git`

Also install FFmpeg backend dependencies:

`apt install libavcodec-dev libavformat-dev libavutil-dev libswresample-dev libswscale-dev`

Now build everything using:

`cmake -S . -B out`
