cmake_minimum_required(VERSION 3.15)
project(NSM)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)

include_directories(math/ filesystem/ window/)

file(GLOB SOURCES "test/*.cpp")

add_executable(NSM ${SOURCES})

target_include_directories(NSM PRIVATE math/ filesystem/ window/)
target_link_libraries(NSM PRIVATE OpenGL::GL)