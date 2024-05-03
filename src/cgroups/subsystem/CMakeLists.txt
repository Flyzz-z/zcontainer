add_library(zcontainer_cgroups_subsystem OBJECT 
subsystem.cc 
cpu_subsystem.cc 
memory_subsystem.cc
cpuset_subsystem.cc
)

set(ALL_OBJECT_FILES
		${ALL_OBJECT_FILES} $<TARGET_OBJECTS:zcontainer_cgroups_subsystem>
		PARENT_SCOPE)