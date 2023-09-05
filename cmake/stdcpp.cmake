set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clang-tidy

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Extra warnings for ${CMAKE_CXX_COMPILER_ID} compiler")
    add_compile_options(-Wpedantic -Wall -Wextra -Wno-cast-function-type)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Extra warnings for ${CMAKE_CXX_COMPILER_ID} compiler")
    add_compile_options(-Wpedantic -Wall -Wextra -Wno-cast-function-type)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Extra warnings for ${CMAKE_CXX_COMPILER_ID} compiler")
    add_compile_options(
      /permissive- # strict standards
      /Wall -wd4191 -wd4668 -wd4710 -wd4711  -wd5045)
    add_compile_definitions(__STDC_LIB_EXT1__ NOMINMAX) # enable fopen_s, kill min/max macros
else()
    message(STATUS "No extra warnings for ${CMAKE_CXX_COMPILER_ID} compiler")
endif()
