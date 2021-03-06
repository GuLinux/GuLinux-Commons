function(add_gtest)
  set(oneValueArgs NAME TYPE ADD_DEPENDENCY)
  set(multiValueArgs SOURCES TARGET_LINK_LIBRARIES)
  cmake_parse_arguments(add_gtest "${options}"  "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")
  set(test_name test_${add_gtest_TYPE}_${add_gtest_NAME})
  add_executable(${test_name} ${add_gtest_SOURCES})
  target_link_libraries(${test_name} gtest gtest_main ${add_gtest_TARGET_LINK_LIBRARIES} pthread)
  if(add_gtest_ADD_DEPENDENCY)
    add_dependencies(${add_gtest_ADD_DEPENDENCY} ${test_name})
  endif(add_gtest_ADD_DEPENDENCY)
  add_test(NAME ${add_gtest_NAME} COMMAND ${test_name})
  #GTEST_ADD_TESTS(${test_name} "" ${add_gtest_SOURCES})
endfunction(add_gtest)
