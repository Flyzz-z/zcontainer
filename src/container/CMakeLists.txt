add_library(zcontainer_container OBJECT container.cc)

set(ALL_OBJECT_FILES
		${ALL_OBJECT_FILES} $<TARGET_OBJECTS:zcontainer_container>
		PARENT_SCOPE)