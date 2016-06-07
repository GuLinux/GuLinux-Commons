if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")
  include_directories(.. googletest/googletest/include/)

  add_executable(test_containers_streams test_containers_streams.cpp)
  target_link_libraries(test_containers_streams gtest_main)
  add_test(test_containers_streams test_containers_streams)

  add_executable(test_optional test_optional.cpp)
  target_link_libraries(test_optional gtest_main)
  add_test(test_optional test_optional)

endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")