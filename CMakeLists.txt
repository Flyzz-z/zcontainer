cmake_minimum_required(VERSION 3.0)

project(zcontainer VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)


set(ZCONTAINER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(ZCONTAINER_THIRD_PARTY_INCLUDE_DIR 
	${PROJECT_SOURCE_DIR}/third_party
	${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include
	${PROJECT_SOURCE_DIR}/third_party/googletest/googlemock/include
	)

include_directories(${ZCONTAINER_INCLUDE_DIR})
include_directories(BEFORE src)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(third_party)

enable_testing()
