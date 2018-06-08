#!/bin/sh

if [ "Darwin" = `uname` ]; then
    export DYLD_LIBRARY_PATH=$PWD/libs/ffmpeg/lib:$PWD/libs/opencv/lib:$DYLD_LIBRARY_PATH
else
    export LD_LIBRARY_PATH=$PWD/libs/ffmpeg/lib:$PWD/libs/opencv/lib:$LD_LIBRARY_PATH
    export PKG_CONFIG_PATH=$PWD/libs/ffmpeg/lib/pkgconfig:$PKG_CONFIG_PATH
    export PKG_CONFIG_LIBDIR=$PWD/libs/ffmpeg/lib
fi
