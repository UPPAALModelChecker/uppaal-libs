#!/usr/bin/env bash

set -e

CORES=$(nproc)

# Debug build, useful when debugging the library:
BUILD_DIR=cmake-linux64-debug
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$BUILD_DIR" -j$CORES
## Test the build:
(cd "$BUILD_DIR" ; ctest --output-on-failure -j$CORES)
## Create a link to it:
if [ ! -e libtable-dbg.so ]; then
	ln -s "$BUILD_DIR"/libtable.so libtable-dbg.so
fi

# Optimized release build:
BUILD_DIR=cmake-linux64-release
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" -j$CORES
## Test the build:
(cd "$BUILD_DIR" ; ctest --output-on-failure -j$CORES)
## Create a link to it:
if [ ! -e libtable.so ]; then
	ln -s "$BUILD_DIR"/libtable.so libtable.so
fi
