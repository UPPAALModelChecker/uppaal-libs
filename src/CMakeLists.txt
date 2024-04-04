add_custom_target(data
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/table_input.csv ${CMAKE_CURRENT_BINARY_DIR}/table_input.csv
        DEPENDS ${PROJECT_SOURCE_DIR}/table_input.csv
        BYPRODUCTS table_input.csv)

add_library(errors OBJECT errors.cpp)

add_library(table SHARED table.cpp)
target_link_libraries(table PRIVATE errors)
add_dependencies(table data)

add_library(array SHARED array.cpp)
target_link_libraries(array PRIVATE errors)

if (UPPAALLIBS_WITH_TESTS)
    add_executable(array_test array_test.cpp)
    target_link_libraries(array_test PRIVATE doctest::doctest_with_main)
    add_dependencies(array_test array)
    add_test(NAME array_test COMMAND array_test)

    add_executable(table_test table_test.cpp)
    target_link_libraries(table_test PRIVATE doctest::doctest_with_main)
    add_dependencies(table_test table)
    add_test(NAME table_test COMMAND table_test)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_executable(errors_test errors_test.cpp)
        target_link_libraries(errors_test PRIVATE errors doctest::doctest_with_main)
        add_test(NAME errors_test COMMAND errors_test)
    endif()
endif (UPPAALLIBS_WITH_TESTS)