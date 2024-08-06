add_subdirectory(cgroups)
add_subdirectory(container)
add_subdirectory(network)

set(ZCONTAINER_LIBS
		zcontainer_container
		zcontainer_cgroups
		zcontainer_cgroups_subsystem
		zcontainer_network
)

add_executable(zcontainer_cli main.cc easylogging++.cc ${ALL_OBJECT_FILES})
target_link_libraries(
	zcontainer_cli
	${ZCONTAINER_LIBS}
)
target_include_directories(zcontainer_cli PUBLIC
	${ZCONTAINER_INCLUDE_DIR}
)

add_library(zcontainer_lib STATIC easylogging++.cc ${ALL_OBJECT_FILES})
target_link_libraries(zcontainer_lib ${ZCONTAINER_LIBS})
target_include_directories(zcontainer_lib PUBLIC
	${ZCONTAINER_INCLUDE_DIR}
)





