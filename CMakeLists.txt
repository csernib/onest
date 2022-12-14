cmake_minimum_required(VERSION 3.4.1)

project(onest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(UNICODE)

set(
	SOURCES
	src/calc/CategoryFactory.cpp
	src/calc/ONEST.cpp
	src/csv/Exporter.cpp
	src/csv/Parser.cpp
	src/gui/AboutDialog.cpp
	src/gui/CategoryGrid.cpp
	src/gui/Diagram.cpp
	src/gui/MainFrame.cpp
	src/gui/ResultGrid.cpp
	src/gui/Table.cpp
	src/io/File.cpp
	src/rule/parser/ParseIntervalRule.cpp
	src/rule/parser/Util.cpp
	src/rule/Categorizer.cpp
	src/rule/Parser.cpp
	src/git.cpp
	src/main.cpp
)

set(
	HEADERS
	src/calc/AssessmentMatrix.h
	src/calc/Category.h
	src/calc/CategoryFactory.h
	src/calc/ONEST.h
	src/csv/Exporter.h
	src/csv/Parser.h
	src/csv/ParserException.h
	src/csv/Sheet.h
	src/gui/rsc/calculation_toggle_100.h
	src/gui/rsc/calculation_toggle_all.h
	src/gui/rsc/dice.h
	src/gui/rsc/header_toggle.h
	src/gui/AboutDialog.h
	src/gui/CategoryGrid.h
	src/gui/ColumnNameGenerator.h
	src/gui/Diagram.h
	src/gui/MainFrame.h
	src/gui/ResultGrid.h
	src/gui/Table.h
	src/gui/common.h
	src/io/File.h
	src/rule/parser/ParseIntervalRule.h
	src/rule/parser/ParseLiteralRule.h
	src/rule/parser/ParseOrRule.h
	src/rule/parser/Util.h
	src/rule/Categorizer.h
	src/rule/IntervalRule.h
	src/rule/LiteralRule.h
	src/rule/OrRule.h
	src/rule/Parser.h
	src/rule/Rule.h
	src/Exception.h
	src/git.h
)

set(
	WIN32_RESOURCES
	src/gui/rsc/icon.rc
)

add_executable(
	onest WIN32
	${SOURCES}
	${HEADERS}
	${WIN32_RESOURCES}
)

set(CMAKE_ONEST_APPLICATION_NAME "ONEST Calculator")
set_target_properties(onest PROPERTIES OUTPUT_NAME ${CMAKE_ONEST_APPLICATION_NAME})

target_compile_definitions(onest PRIVATE CMAKE_ONEST_APPLICATION_NAME="${CMAKE_ONEST_APPLICATION_NAME}")
target_compile_definitions(onest PRIVATE CMAKE_ONEST_CONTACT_EMAIL="public.balint.cserni@gmail.com")
target_compile_definitions(onest PRIVATE CMAKE_ONEST_PROJECT_PAGE="https://github.com/csernib/onest/")

file(STRINGS VERSION CMAKE_ONEST_APPLICATION_VERSION ENCODING UTF-8)
target_compile_definitions(onest PRIVATE CMAKE_ONEST_APPLICATION_VERSION="${CMAKE_ONEST_APPLICATION_VERSION}")

file(STRINGS LICENSE CMAKE_ONEST_LICENSE_TEXT ENCODING UTF-8)
list(JOIN CMAKE_ONEST_LICENSE_TEXT [=[\n]=] CMAKE_ONEST_LICENSE_TEXT)
list(PREPEND CMAKE_ONEST_LICENSE_TEXT [=[R"___(]=])
list(APPEND CMAKE_ONEST_LICENSE_TEXT [=[)___"]=])
list(JOIN CMAKE_ONEST_LICENSE_TEXT "" CMAKE_ONEST_LICENSE_TEXT)
target_compile_definitions(onest PRIVATE CMAKE_ONEST_LICENSE_TEXT=${CMAKE_ONEST_LICENSE_TEXT})

add_subdirectory(test)


set(wxUSE_STL ON)
set(wxBUILD_SHARED OFF)
add_subdirectory(ext/wx-widgets)
target_include_directories(onest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ext/wx-widgets/include)
target_link_libraries(onest wx::core wx::html)

add_subdirectory(ext/git)
target_link_libraries(onest cmake_git_version_tracking)
