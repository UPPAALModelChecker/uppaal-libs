add_custom_target(data
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/table_input.csv ${CMAKE_CURRENT_BINARY_DIR}/table_input.csv
        DEPENDS ${PROJECT_SOURCE_DIR}/table_input.csv
        BYPRODUCTS table_input.csv)

add_library(errors OBJECT errors.cpp)

add_library(table SHARED table.cpp)
target_link_libraries(table PRIVATE errors)
add_dependencies(table data)

if (UPPAALLIBS_WITH_TESTS)
    add_executable(test_table test_table.cpp)
    target_link_libraries(test_table PRIVATE doctest::doctest_with_main)
    add_dependencies(test_table table)
    add_test(NAME test_table COMMAND test_table)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_executable(test_errors test_errors.cpp)
        target_link_libraries(test_errors PRIVATE errors doctest::doctest_with_main)
        add_test(NAME test_errors COMMAND test_errors)
    endif()
endif (UPPAALLIBS_WITH_TESTS)