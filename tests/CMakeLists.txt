add_definitions(-DBOOST_TEST_DYN_LINK)

add_executable(json_object_test json_object_test)
target_link_libraries(json_object_test Wt_Commons_Library ${Wt_LIBRARIES} ${Boost_LIBRARIES})
add_test(unit_json_object_test json_object_test)
