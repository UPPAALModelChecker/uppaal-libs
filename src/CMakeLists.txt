add_library(logging OBJECT logging.cpp)

add_library(table SHARED table.cpp)
target_link_libraries(table PRIVATE logging)
