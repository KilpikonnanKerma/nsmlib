cmake_minimum_required(VERSION 3.15)
project(NSM)

set(CMAKE_CXX_STANDARD 17)

include_directories(nsm)

file(GLOB SOURCES "test/*.cpp")

add_executable(NSM ${SOURCES})

target_compile_definitions(NSM PRIVATE GLM_ENABLE_EXPERIMENTAL)

target_include_directories(NSM PRIVATE nsm)