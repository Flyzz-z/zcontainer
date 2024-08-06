add_library(zcontainer_network_driver OBJECT bridge_network_driver.cc)

set(ALL_OBJECT_FILES
		${ALL_OBJECT_FILES} $<TARGET_OBJECTS:zcontainer_network_driver>
		PARENT_SCOPE)