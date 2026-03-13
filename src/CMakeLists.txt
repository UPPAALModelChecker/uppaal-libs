add_library(logging OBJECT logging.cpp)
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_definitions(logging PUBLIC LOGGING=1)
else() # assuming multi-config generator:
    target_compile_definitions(logging PUBLIC $<$<CONFIG:Debug,RelWithDebInfo>:LOGGING=1> )
endif ()


add_library(table SHARED table.cpp)
target_link_libraries(table PRIVATE logging)
