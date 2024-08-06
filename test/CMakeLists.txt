include(GoogleTest)

add_executable(ipam_test
		network/ipam_test.cc
)

target_link_libraries(ipam_test
		gtest_main
		gmock_main
		zcontainer_lib
)

set_target_properties(ipam_test
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
)

add_executable(ip_rtnl_manager_test
		network/rtnl/ip_rtnl_manager_test.cc
)

target_link_libraries(ip_rtnl_manager_test
		gtest_main
		gmock_main
		zcontainer_lib
)

set_target_properties(ip_rtnl_manager_test
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
)