set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clang-tidy

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Adding extra warnings for ${CMAKE_CXX_COMPILER_ID} compiler")
    add_compile_options(-Wall -Wextra -Wno-cast-function-type)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Adding extra warnings for ${CMAKE_CXX_COMPILER_ID} compiler")
    add_compile_options(-Wall -Wextra -Wno-cast-function-type)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Adding extra warnings for ${CMAKE_CXX_COMPILER_ID} compiler")
    #add_compile_options(/permissive- /WX /Wall)
	add_compile_options(/Wall -wd5045 -wd4668)
    add_compile_definitions(__STDC_LIB_EXT1__) # enable fopen_s et al
else()
    message(WARNING "No extra warnings for ${CMAKE_CXX_COMPILER_ID} compiler")
endif()
