cmake_minimum_required(VERSION 3.19)

#
# Project details
#

project(
  "USTC_CG_2024"
  VERSION 1.0
  LANGUAGES C CXX
)

message(STATUS "Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...\n")
set(BUILD_SHARED_LIBS ON)
# On mac, the cpp standard should >= 20
set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()


set(OUT_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Binaries/${CMAKE_BUILD_TYPE})
message("Build binaries to ${OUT_BINARY_DIR}")
set(OUTPUT_DIR 
  RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${OUT_BINARY_DIR}"
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUT_BINARY_DIR}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${OUT_BINARY_DIR}"
)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  set(SDK_FOLDER ${CMAKE_BUILD_TYPE})
else()
  set(SDK_FOLDER "Release")
endif()

message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}. Selecting SDK from ${SDK_FOLDER}") 

set(pxr_DIR SDK/${SDK_FOLDER}/OpenUSD)
set(TBB_DIR SDK/${SDK_FOLDER}/tbb/lib/cmake/tbb)
set(MaterialX_DIR SDK/${SDK_FOLDER}/MaterialX/lib/cmake/MaterialX)
set(Imath_DIR SDK/${SDK_FOLDER}/Imath/lib/cmake/Imath)
set(EMBREE_DIR SDK/common/embree/lib/cmake/embree-4.3.1) 

find_package(Python3 3.10 EXACT COMPONENTS Development Interpreter REQUIRED)
set_target_properties(Python3::Python PROPERTIES MAP_IMPORTED_CONFIG_DEBUG ";RELEASE")

find_package(pxr REQUIRED)
find_package(TBB REQUIRED)
find_package(EMBREE REQUIRED)
set(TBB_tbb_LIBRARY TBB::tbb TBB::tbbmalloc TBB::tbbmalloc_proxy)
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	set_target_properties(TBB::tbb PROPERTIES INTERFACE_COMPILE_DEFINITIONS "TBB_USE_DEBUG=1")
endif()

add_subdirectory(external)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/GCore)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/SCore)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/RCore)
target_link_libraries(SCore PUBLIC GCore)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/GUI)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/nodes)

# For testing
enable_testing()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/googletest)
set_target_properties(gtest PROPERTIES ${OUTPUT_DIR})
set_target_properties(gtest_main PROPERTIES ${OUTPUT_DIR})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)