cmake_minimum_required(VERSION 3.4.1)

project(onest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(UNICODE)

set(
	SOURCES
	src/calc/ONEST.cpp
	src/category/CategoryFactory.cpp
	src/csv/Parser.cpp
	src/gui/MainFrame.cpp
	src/gui/Table.cpp
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
	src/csv/Sheet.h
	src/gui/MainFrame.h
	src/gui/Table.h
	src/io/File.h
	src/Exception.h
)

add_executable(
	onest WIN32
	${SOURCES}
	${HEADERS}
)

add_subdirectory(test)


set(wxUSE_STL ON)
set(wxBUILD_SHARED OFF)
add_subdirectory(ext/wx-widgets)
target_include_directories(onest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ext/wx-widgets/include)
target_link_libraries(onest wx::core)
