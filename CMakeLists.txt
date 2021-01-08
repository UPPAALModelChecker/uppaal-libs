cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clang-tidy
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC

project(uppaal_libs CXX)

add_custom_target(data
        COMMAND cp ${CMAKE_SOURCE_DIR}/table_input.csv ${PROJECT_BINARY_DIR}/table_input.csv
        DEPENDS table_input.csv
        BYPRODUCTS table_input.csv)

add_library(errors OBJECT errors.cpp)

add_library(table SHARED table.cpp)
target_compile_options(table PRIVATE -Wall -Wextra)
target_link_options(table PRIVATE -static-libstdc++ -static-libgcc -rdynamic)
target_link_libraries(table errors)
add_dependencies(table data)

add_executable(test_table test_table.cpp)
target_link_options(test_table PRIVATE -static-libstdc++ -static-libgcc)
target_link_libraries(test_table PRIVATE dl)
add_dependencies(test_table table)

enable_testing()
add_test(NAME test_table COMMAND test_table)
