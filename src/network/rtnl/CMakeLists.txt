add_library(zcontainer_network_rtnl OBJECT ip_rtnl_manager.cc)

set(ALL_OBJECT_FILES
		${ALL_OBJECT_FILES} $<TARGET_OBJECTS:zcontainer_network_rtnl>
		PARENT_SCOPE)