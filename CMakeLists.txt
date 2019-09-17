cmake_minimum_required(VERSION 3.14)
project(cpp_lab)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp_lab main.cpp)
add_executable(lab2 lab2.cpp)
add_executable(lab3 lab3.cpp)
add_executable(lab4 lab4.cpp)
add_executable(lab4.2 lab4.2.cpp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(lab5 lab_5.cpp)

find_package(Qt5Widgets REQUIRED)

target_link_libraries(lab5 Qt5::Widgets)