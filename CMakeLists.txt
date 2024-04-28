cmake_minimum_required(VERSION 3.0)

project(zcontainer VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

add_executable(zcontainer 
	main.cc
	easylogging++.cc
	container/container.cc
)