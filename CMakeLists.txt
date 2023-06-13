cmake_minimum_required(VERSION 3.15)

include(cmake/stdcpp.cmake)

project(uppaal_libs CXX)

include(cmake/sanitizers.cmake)

option(UPPAALLIBS_WITH_TESTS "UPPAAL LIBS Unit Tests" ON)

if (UPPAALLIBS_WITH_TESTS)
    include(cmake/doctest.cmake)
    enable_testing()
endif (UPPAALLIBS_WITH_TESTS)

add_subdirectory(src)
