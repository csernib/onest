cmake_minimum_required(VERSION 3.4.1)

project(onest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
	SOURCES
	src/main.cpp
	src/csv/Parser.cpp
	src/io/File.cpp
)

set(
	HEADERS
	src/csv/Parser.h
	src/csv/ParserException.h
	src/io/File.h
)

add_executable(
	onest
	${SOURCES}
	${HEADERS}
)

add_subdirectory(test)
