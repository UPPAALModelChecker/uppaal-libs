#!/usr/bin/env bash

mkdir -p cmake-linux64-debug
cd cmake-linux64-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j$(nproc)
ctest --output-on-failure -j$(nproc)
