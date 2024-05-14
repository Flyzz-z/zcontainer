add_subdirectory(cgroups)
add_subdirectory(container)


add_executable(zcontainer main.cc easylogging++.cc ${ALL_OBJECT_FILES})

set(ZCONTAINER_LIBS
		zcontainer_container
		zcontainer_cgroups
		zcontainer_cgroups_subsystem
)

target_link_libraries(
	zcontainer
	${ZCONTAINER_LIBS}
)

target_include_directories(zcontainer PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)




