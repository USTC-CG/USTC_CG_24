# FindNVAPI.cmake

# Locate NVAPI library
# This module defines
# NVAPI::NVAPI target to include NVAPI headers and libraries

find_path(NVAPI_INCLUDE_DIR
  NAMES nvapi.h
  PATHS ${CMAKE_SOURCE_DIR}/external/nvapi
  PATH_SUFFIXES amd64
  NO_DEFAULT_PATH
)

find_library(NVAPI_LIBRARY
  NAMES nvapi64
  PATHS ${CMAKE_SOURCE_DIR}/external/nvapi
  PATH_SUFFIXES amd64
  NO_DEFAULT_PATH
)

if (NVAPI_INCLUDE_DIR AND NVAPI_LIBRARY)
  set(NVAPI_FOUND TRUE)
else()
  set(NVAPI_FOUND FALSE)
endif()

if (NVAPI_FOUND)
  add_library(NVAPI::NVAPI INTERFACE IMPORTED)
  set_target_properties(NVAPI::NVAPI PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${NVAPI_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${NVAPI_LIBRARY}"
  )
  message(STATUS "Found NVAPI: ${NVAPI_LIBRARY}")
else()
  message(WARNING "NVAPI not found")
endif()

mark_as_advanced(NVAPI_INCLUDE_DIR NVAPI_LIBRARY)
