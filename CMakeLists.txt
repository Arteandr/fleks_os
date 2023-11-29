cmake_minimum_required(VERSION 3.15)

project(OSCoursework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Curses REQUIRED)


file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
include_directories(${CURSES_INCLUDE_DIR})
add_executable(main ${SRC_FILES} main.cpp)
target_include_directories(main PRIVATE includes/)
target_link_libraries(main ${CURSES_LIBRARIES})
target_compile_options(main PRIVATE -Wall)
target_compile_options(main PRIVATE -lncurses)
