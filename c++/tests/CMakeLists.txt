if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")
  add_subdirectory(googletest)
  LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/")
  message("Modules path: ${CMAKE_MODULE_PATH}")
  include(add_gtest)
  include_directories(.. googletest/googletest/include/)
  
  add_gtest(NAME containers_stream TYPE unit SOURCES test_containers_streams.cpp )
  add_gtest(NAME test_optional TYPE unit SOURCES test_optional.cpp )

else(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")
  message(WARNING "Tests are enabled but googletest submodule was not found")
endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")