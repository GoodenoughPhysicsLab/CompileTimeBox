cmake_minimum_required(VERSION 3.15)

project(tests)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
enable_testing()

file(GLOB_RECURSE TEST_SRCS ${CMAKE_SOURCE_DIR}/*.cc)

include_directories(${CMAKE_SOURCE_DIR}/../include)

if (MSVC)
    add_compile_options(/Zc:preprocessor /utf-8 /DNOMINMAX /D_USE_MATH_DEFINES /bigobj)
else()
    add_compile_options(-Wall -Wextra -Wno-parentheses)
    if (WIN32)
        add_compile_options(-finput-charset=utf-8 -fexec-charset=utf-8)
    endif()
endif()

foreach(a_test IN LISTS TEST_SRCS)
    get_filename_component(filename ${a_test} NAME_WE)
    add_executable(${filename} ${a_test})
    add_test(NAME ${filename} COMMAND ${CMAKE_BINARY_DIR}/${filename})
endforeach()
