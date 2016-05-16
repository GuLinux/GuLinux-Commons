include_directories(${GTEST_INCLUDE_DIRS} ..)

add_executable(test_containers_streams test_containers_streams.cpp)
target_link_libraries(test_containers_streams ${GTEST_BOTH_LIBRARIES})
add_test(test_containers_streams test_containers_streams)

add_executable(test_optional test_optional.cpp)
target_link_libraries(test_optional ${GTEST_BOTH_LIBRARIES})
add_test(test_optional test_optional)
