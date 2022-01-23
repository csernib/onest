cmake_minimum_required(VERSION 3.4.1)

project(onest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
	SOURCES
	src/calc/ONEST.cpp
	src/category/CategoryFactory.cpp
	src/csv/Parser.cpp
	src/io/File.cpp
	src/main.cpp
)

set(
	HEADERS
	src/calc/AssessmentMatrix.h
	src/calc/ONEST.h
	src/category/Category.h
	src/category/CategoryFactory.h
	src/category/CategoryMatcher.h
	src/category/DiscreteCategoryMatcher.h
	src/category/RangeBasedCategoryMatcher.h
	src/csv/Parser.h
	src/csv/ParserException.h
	src/io/File.h
	src/Exception.h
)

add_executable(
	onest
	${SOURCES}
	${HEADERS}
)

add_subdirectory(test)
