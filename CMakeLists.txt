cmake_minimum_required(VERSION 3.15)

include(cmake/stdcpp.cmake)

project(uppaal_libs CXX)

include(cmake/sanitizers.cmake)

enable_testing()

add_subdirectory(src)
