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
  add_library(${lib_name} STATIC ${output_file})
endfunction()

file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE NODE_FILES ${CMAKE_CURRENT_LIST_DIR}/nodes/*.cpp)

add_node_discovery(
  nodes_generated
  "${NODE_FILES}"
  ${CMAKE_CURRENT_BINARY_DIR}/register_nodes.cc
  register_nodes 
)

add_library(nodes STATIC "${CPP_FILES}")

target_link_libraries(nodes_generated nodes)
target_link_libraries(nodes nodes_generated)

target_link_libraries(nodes GCore)
target_link_libraries(nodes Eigen3::Eigen)
target_include_directories(nodes PRIVATE ${CMAKE_CURRENT_LIST_DIR}/intern)

target_compile_options(nodes PRIVATE -DUSTC_CG_BUILD_MODULE=1 -DNOMINMAX)

