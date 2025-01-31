cmake_minimum_required(VERSION 3.15)

project(detect-features LANGUAGES CXX)
# CMAKE_CXX_STANDARD will be defined externally

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/main.cc)
