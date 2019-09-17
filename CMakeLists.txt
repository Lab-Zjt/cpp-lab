cmake_minimum_required(VERSION 3.10)
project(cpp_lab)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    message(STATUS "Using GBK charset in Windows")
    set(CMAKE_CXX_FLAGS "-fexec-charset=GBK -finput-charset=UTF-8 -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
endif()

add_executable(cpp_lab main.cpp)
add_executable(lab2 lab2.cpp)
add_executable(lab3 lab3.cpp)
add_executable(lab4 lab4.cpp)