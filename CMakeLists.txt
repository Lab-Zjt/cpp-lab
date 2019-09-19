cmake_minimum_required(VERSION 3.10)
project(cpp_lab)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    message(STATUS "Using GBK charset in Windows")
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH} D:\\Qt\\5.12.2\\mingw73_64\\lib\\cmake")
endif()

add_executable(lab1 lab1.cpp)
add_executable(lab2 lab2.cpp)
add_executable(lab3 lab3.cpp)
add_executable(lab4 lab4.cpp)
add_executable(lab4.2 lab4.2.cpp)

add_executable(lab5 lab_5.cpp)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

target_link_libraries(lab5 Qt5::Widgets)