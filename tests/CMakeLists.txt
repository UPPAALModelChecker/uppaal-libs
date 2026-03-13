add_executable(table_test table_test.cpp)
target_link_libraries(table_test PRIVATE doctest::doctest_with_main)
add_dependencies(table_test table)
add_test(NAME table_test COMMAND table_test)

add_executable(logging_test logging_test.cpp)
target_link_libraries(logging_test PRIVATE logging doctest::doctest_with_main)
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_test(NAME logging_test COMMAND logging_test)
else() # assuming multi-config generator:
    add_test(NAME logging_test COMMAND logging_test CONFIGURATION Debug RelWithDebInfo)
endif ()
