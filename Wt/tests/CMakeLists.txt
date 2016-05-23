add_definitions(-DBOOST_TEST_DYN_LINK)

if(HAVE_WT_JSON_SERIALIZE)
  add_executable(json_object_test json_object_test)
  target_link_libraries(json_object_test Wt_Commons_Library ${Wt_LIBRARY} ${Boost_LIBRARIES})
  add_test(unit_json_object_test json_object_test)
  set(run_tests_DEPENDS json_object_test)
endif(HAVE_WT_JSON_SERIALIZE)

add_executable(utils_test wt_utils_test.cpp)
target_link_libraries(utils_test Wt_Commons_Library ${Wt_LIBRARY} ${Boost_LIBRARIES})
add_test(unit_utils_test utils_test)

add_custom_target(run_tests COMMAND ctest --output-on-failure -R "unit.*" DEPENDS ${run_tests_DEPENDS})

