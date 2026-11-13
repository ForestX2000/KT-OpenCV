cmake_minimum_required(VERSION 4.3)
project(KT_OpenCV)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(.)

add_executable(KT_OpenCV
        first.cpp
        first.h)

add_executable(KT_OpenCV_2
        second.cpp
        second.h)

find_package(OpenCV REQUIRED)
target_include_directories(KT_OpenCV PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(KT_OpenCV ${OpenCV_LIBS})

target_include_directories(KT_OpenCV_2 PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(KT_OpenCV_2 ${OpenCV_LIBS})