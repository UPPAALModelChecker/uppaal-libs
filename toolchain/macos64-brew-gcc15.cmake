# the name of the target operating system
set(CMAKE_SYSTEM_NAME Darwin)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER gcc-15)
set(CMAKE_CXX_COMPILER g++-15)
set(CMAKE_AR gcc-ar-15)
set(CMAKE_NM gcc-nm-15)
set(CMAKE_RANLIB gcc-ranlib-15) # https://stackoverflow.com/questions/53128049/ld-archive-has-no-table-of-contents-file-error-with-homebrew
set(RANLIB gcc-ranlib-15)
# silence superfluous "has no symbols" warnings (does not help):
# set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
# set(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")

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
