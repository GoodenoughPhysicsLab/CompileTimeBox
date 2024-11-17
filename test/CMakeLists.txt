cmake_minimum_required(VERSION 3.15)

project(test_project)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
enable_testing()

if (MSVC)
    add_compile_options(/Zc:preprocessor /utf-8 /DNOMINMAX /D_USE_MATH_DEFINES /bigobj)
else()
    add_compile_options(-Wall -Wextra -Wno-parentheses)
    if (WIN32)
        add_compile_options(-finput-charset=utf-8 -fexec-charset=utf-8)
    endif()
endif()

add_executable(${PROJECT_NAME} test.cc)
add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
