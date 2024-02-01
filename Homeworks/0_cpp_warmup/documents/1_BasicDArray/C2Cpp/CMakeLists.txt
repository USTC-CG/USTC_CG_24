cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

project(C2Cpp VERSION 1.0)
message(STATUS "[Project] ${PROJECT_NAME}")

set(CMAKE_CXX_STANDARD 20)

include(cmake/helper.cmake)

set(OUTPUT_PROP   
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_SOURCE_DIR}/bin"
)

add_subdirectory(src)

