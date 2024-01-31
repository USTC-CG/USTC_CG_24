cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(C2Cpp VERSION 1.0)
message(STATUS "[Project] ${PROJECT_NAME}")

set(CMAKE_CXX_STANDARD 20) # 设置C++标准

include(cmake/helper.cmake) # 引入一些cmake函数简化代码

add_subdirectory(src)

