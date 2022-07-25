cmake_minimum_required(VERSION 3.4.1)

project(onest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(UNICODE)

set(
	SOURCES
	src/calc/CategoryFactory.cpp
	src/calc/ONEST.cpp
	src/csv/Parser.cpp
	src/gui/MainFrame.cpp
	src/gui/Table.cpp
	src/io/File.cpp
	src/main.cpp
)

set(
	HEADERS
	src/calc/AssessmentMatrix.h
	src/calc/Category.h
	src/calc/CategoryFactory.h
	src/calc/ONEST.h
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

add_subdirectory(ext/glfw)

add_library(glad ext/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/include)

set(BUILD_EXPERIMENTAL_OPENGL_BACKEND ON)
add_subdirectory(ext/matplot++)
target_include_directories(onest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ext/matplot++/source)
target_link_libraries(onest matplot matplot_opengl)
