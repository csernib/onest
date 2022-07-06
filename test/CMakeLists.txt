cmake_minimum_required(VERSION 3.4.1)

project(onest_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
	SOURCES
	main.cpp
	Test_AssessmentMatrix.cpp
	Test_CategoryFactory.cpp
	Test_File.cpp
	Test_Parser.cpp
)

set(
	PROGRAM_SOURCES
	../src/calc/CategoryFactory.cpp
	../src/csv/Parser.cpp
	../src/io/File.cpp
)

set(
	HEADERS
	test.h
)

add_executable(
	onest_test
	${SOURCES}
	${PROGRAM_SOURCES}
	${HEADERS}
)

target_include_directories(onest_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../ext/lest/include)
