cmake_minimum_required(VERSION 3.0)

project(zcontainer VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)


include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(BEFORE src)
add_subdirectory(src)

