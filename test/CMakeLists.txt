cmake_minimum_required(VERSION 3.4.1)

project(onest_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
	SOURCES
	main.cpp
	Test_AssessmentMatrix.cpp
	Test_CategoryFactory.cpp
	Test_ColumnNameGenerator.cpp
	Test_Exporter.cpp
	Test_File.cpp
	Test_ONEST.cpp
	Test_Parser.cpp
	Test_RuleCategorizer.cpp
	Test_RuleIntervalRule.cpp
	Test_RuleLiteralRule.cpp
	Test_RuleOrRule.cpp
	Test_RuleParserIntervalRule.cpp
	Test_RuleParserOrRule.cpp
	Test_RuleParserUtil.cpp
)

set(
	PROGRAM_SOURCES
	../src/calc/CategoryFactory.cpp
	../src/calc/ONEST.cpp
	../src/csv/Exporter.cpp
	../src/csv/Parser.cpp
	../src/io/File.cpp
	../src/rule/parser/ParseIntervalRule.cpp
	../src/rule/parser/Util.cpp
	../src/rule/Categorizer.cpp
	../src/rule/Parser.cpp
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
