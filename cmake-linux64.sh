#!/usr/bin/env bash

set -e

mkdir -p cmake-linux64-debug
cd cmake-linux64-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j$(nproc)
ctest --output-on-failure -j$(nproc)
cd ..
if [ ! -e libtable-dbg.so ]; then
	ln -s cmake-linux64-debug/libtable.so libtable-dbg.so
fi

mkdir -p cmake-linux64-release
cd cmake-linux64-release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
ctest --output-on-failure -j$(nproc)
cd ..
if [ ! -e libtable.so ]; then
	ln -s cmake-linux64-release/libtable.so libtable.so
fi
