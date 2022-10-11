add_custom_target(data
        COMMAND cp ${PROJECT_SOURCE_DIR}/table_input.csv ${CMAKE_CURRENT_BINARY_DIR}/table_input.csv
        DEPENDS ${PROJECT_SOURCE_DIR}/table_input.csv
        BYPRODUCTS table_input.csv)

add_library(errors OBJECT errors.cpp)

add_library(table SHARED table.cpp)
target_compile_options(table PRIVATE -Wall -Wextra)
target_link_options(table PRIVATE -rdynamic)
target_link_libraries(table errors)
add_dependencies(table data)

add_executable(test_table test_table.cpp)
target_link_options(test_table PRIVATE)
target_link_libraries(test_table PRIVATE dl)
add_dependencies(test_table table)
add_test(NAME test_table COMMAND test_table)
