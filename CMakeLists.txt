cmake_minimum_required(VERSION "3.28.0")

project("LeviathanEngine")

set(TEST_TITLE_NAME "TestTitle")
add_executable("${TEST_TITLE_NAME}" "TestTitle/Source/Private/Main.cpp")
set_property(TARGET "${TEST_TITLE_NAME}" PROPERTY CXX_STANDARD_23)
target_compile_options("${TEST_TITLE_NAME}" PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>) 
set_target_properties("${TEST_TITLE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "Bin/$<CONFIG>")

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "${TEST_TITLE_NAME}")