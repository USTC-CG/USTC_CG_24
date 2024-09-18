# SPDX-FileCopyrightText: 2006 Blender Authors
#
# SPDX-License-Identifier: GPL-2.0-or-later

set(DISCOVER_NODES_PATH ${CMAKE_CURRENT_LIST_DIR}/intern/discover_nodes.py)

# Creates a library with the given name that exposes one function which calls
# the register functions of all nodes in the provided source files.
function(add_node_discovery
  lib_name
  sources
  output_file
  output_function
  )

  set(extra_args "")

  # See `add_definitions(-DUSE_MAKEFILE_WORKAROUND)` comment for `makesrna`.
  if(CMAKE_GENERATOR MATCHES ".*Unix Makefiles")
    set(extra_args "--use-makefile-workaround")
  endif()

  add_custom_command(
    OUTPUT
      ${output_file}
    COMMAND
      ${Python3_EXECUTABLE}
        ${DISCOVER_NODES_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${output_file}
        ${output_function}
        ${sources}
        ${extra_args}
    DEPENDS
      ${sources}
      ${DISCOVER_NODES_PATH}
  )
  set_source_files_properties(${output_file} PROPERTIES GENERATED TRUE)
  set(${lib_name} "${output_file}" PARENT_SCOPE)
endfunction()

file(GLOB_RECURSE 
    CPP_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp 
)

file(GLOB_RECURSE 
    CUDA_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/*.cu
)

file(GLOB_RECURSE 
    NODE_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/nodes/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/nodes/*.cu
)

add_node_discovery(
  nodes_generated
  "${NODE_FILES}"
  ${CMAKE_CURRENT_BINARY_DIR}/register_nodes.cc
  register_nodes 
)

add_library(nodes STATIC ${CPP_FILES} ${nodes_generated})

add_library(nodes_cuda STATIC "${CUDA_FILES}")
message("Torch has libraries: ${${TORCH_LIBRARIES}}")
target_link_libraries(nodes_cuda PUBLIC "${TORCH_LIBRARIES}")
target_link_libraries(nodes_cuda PUBLIC glm::glm-header-only)


target_link_libraries(nodes PUBLIC nodes_cuda)
target_link_libraries(nodes PUBLIC GCore RCore)
target_link_libraries(nodes PUBLIC Eigen3::Eigen)
target_link_libraries(nodes PUBLIC Python3::Python)
target_link_libraries(nodes PUBLIC hioOpenVDB EnTT::EnTT)
target_link_libraries(nodes PUBLIC hioOpenVDB)
target_link_libraries(nodes PUBLIC nvrhi DXC::dxc)

target_include_directories(nodes PRIVATE ${CMAKE_CURRENT_LIST_DIR}/intern)
target_include_directories(nodes PRIVATE ${PROJECT_SOURCE_DIR}/source/RCore/hd_USTC_CG_GL)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(nodes PUBLIC OpenMP::OpenMP_CXX)
    message(STATUS "OpenMP found, building with it.")
else()
    message(WARNING "OpenMP not found, building without it.")
endif()

target_compile_options(nodes PRIVATE -DUSTC_CG_BUILD_MODULE=0 -DNOMINMAX)

get_filename_component(NODES_FILES_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
set(NODES_FILES_DIR_MACRO "#define NODES_FILES_DIR \"${NODES_FILES_DIR}\"")
set(RENDER_NODES_FILES_DIR_MACRO "#define RENDER_NODES_FILES_DIR \"${NODES_FILES_DIR}/nodes/render\"")
set(FUNC_NODES_FILES_DIR_MACRO "#define FUNC_NODES_FILES_DIR \"${NODES_FILES_DIR}/nodes/functions\"")
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/NODES_FILES_DIR.h.in"
    "${CMAKE_CURRENT_LIST_DIR}/nodes/render/NODES_FILES_DIR.h"
)

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/NODES_FILES_DIR.h.in"
    "${CMAKE_CURRENT_LIST_DIR}/nodes/functions/NODES_FILES_DIR.h"
)