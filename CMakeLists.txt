set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

cmake_minimum_required(VERSION 2.6)
project(kdev_jasm)

add_executable(jasm jasm.h parse.cpp print.cpp stack.cpp utils.cpp Variable.h)

install(TARGETS rjc RUNTIME DESTINATION bin)
