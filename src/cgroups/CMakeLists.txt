add_subdirectory(subsystem)

add_library(zcontainer_cgroups OBJECT cgroups_manager.cc)

set(ALL_OBJECT_FILES
		${ALL_OBJECT_FILES} $<TARGET_OBJECTS:zcontainer_cgroups>
		PARENT_SCOPE)