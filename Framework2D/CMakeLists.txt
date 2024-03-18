cmake_minimum_required(VERSION 3.14)

project(
  "USTC_CG_2024_Framework2D"
  VERSION 1.0
  LANGUAGES C CXX
)
add_compile_options(-std=c++20)
add_compile_options(-msse2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

message(STATUS "Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...\n")
#set(BUILD_SHARED_LIBS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(FRAMEWORK2D_DIR "${PROJECT_SOURCE_DIR}")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(THIRD_PARTY_DIR "${PROJECT_SOURCE_DIR}/third_party")
set(BINARY_DIR "${PROJECT_SOURCE_DIR}/bin")
set(LIBRARY_DIR "${PROJECT_SOURCE_DIR}/libs")

add_subdirectory(third_party)
add_subdirectory(src)