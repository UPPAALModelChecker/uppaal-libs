#!/usr/bin/env bash

set -e

CORES=$(sysctl -n hw.ncpu)

# Debug build, useful when debugging the library:
BUILD_DIR=build-darwin-debug
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DUBSAN=ON -DASAN=ON
cmake --build "$BUILD_DIR" -j$CORES
## Test the build:
(cd "$BUILD_DIR" ; ctest --output-on-failure -j$CORES)
## Create a link to it:
if [ ! -e libtable-dbg.dylib ]; then
	ln -s "$BUILD_DIR"/src/libtable.dylib libtable-dbg.dylib
fi

# Optimized release build:
BUILD_DIR=build-darwin-release
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" -j$CORES
## Test the build:
(cd "$BUILD_DIR" ; ctest --output-on-failure -j$CORES)
## Create a link to it:
if [ ! -e libtable.dylib ]; then
	ln -s "$BUILD_DIR"/src/libtable.dylib libtable.dylib
fi
