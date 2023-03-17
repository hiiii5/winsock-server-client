cmake_minimum_required(VERSION 3.10)

project(NetworkingProject)

file(GLOB SOURCES
	./source/*.h
	./source/*.cpp
)

if (WIN32)
	add_executable (NetworkingProject ${SOURCES})
endif(WIN32)
