add_subdirectory(driver)
add_subdirectory(rtnl)


add_library(zcontainer_network OBJECT ip_net.cc ipam.cc network_manager.cc network.cc)

set(ALL_OBJECT_FILES
		${ALL_OBJECT_FILES} $<TARGET_OBJECTS:zcontainer_network>
		PARENT_SCOPE)