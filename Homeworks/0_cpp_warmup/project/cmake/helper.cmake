function(AddExeWithFile exe_name)
  FILE(GLOB_RECURSE HEADER "*.h" )
  FILE(GLOB_RECURSE SOURCE "*.cpp" )
  message(STATUS "Header files: ${HEADER}")
  message(STATUS "Source files: ${SOURCE}")

  source_group("Headers" FILES ${HEADER})
  source_group("Sources" FILES ${SOURCE})

  add_executable(${exe_name} ${SOURCE} ${HEADER})
endfunction()