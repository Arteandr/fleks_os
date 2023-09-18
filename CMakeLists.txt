cmake_minimum_required(VERSION 3.15)

project(OSCoursework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE SRC_FILES src/*.c)
add_executable(main main.c ${SRC_FILES})

target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)
