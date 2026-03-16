# the name of the target operating system
set(CMAKE_SYSTEM_NAME Darwin)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER cc)
set(CMAKE_CXX_COMPILER c++)

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH "${CMAKE_PREFIX_PATH}")
# Do not use RPATH:
#set(CMAKE_MACOSX_RPATH FALSE)
#set(MACOSX_RPATH FALSE)
# set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# set(CMAKE_SKIP_RPATH TRUE)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment,
# search programs in both the target and host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
