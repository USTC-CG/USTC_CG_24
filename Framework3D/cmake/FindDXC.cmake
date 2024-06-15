message("dxc downloaded files at ${DXC_ROOT_DIR}")

include(FindPackageHandleStandardArgs)

# Define search paths based on user input and environment variables
set(DXC_SEARCH_DIR ${DXC_ROOT_DIR}/inc)
##################################
# Find the DXC include dir
##################################

find_path(DXC_INCLUDE_DIR dxcapi.h
    HINTS ${DXC_SEARCH_DIR})

set(DXC_ROOT_DIR ${DXC_INCLUDE_DIR}/..)
set(DXC_LIBRARY_DIR ${DXC_ROOT_DIR}/lib/x64)

set(DXC_SEARCH_COMPOMPONENTS dxc)
set(DXC_LIB_PATH_SUFFIX "lib/x64")
# Find each component

message("DXC_LIBRARY_DIR: ${DXC_LIBRARY_DIR}")
# Search for the libraries
find_library(DXC_dxcompiler_LIBRARY dxcompiler
    HINTS ${DXC_LIBRARY_DIR} ${DXC_ROOT_DIR}
    PATH_SUFFIXES ${DXC_LIB_PATH_SUFFIX})

if(DXC_dxcompiler_LIBRARY)
list(APPEND DXC_LIBRARIES "${DXC_dxcompiler_LIBRARY}")
endif()

if(DXC_dxcompiler_LIBRARY AND EXISTS "${DXC_dxcompiler_LIBRARY}")
set(DXC_dxcompiler_FOUND TRUE)
else()
set(DXC_dxcompiler_FOUND FALSE)
endif()
# Mark internal variables as advanced
mark_as_advanced(DXC_dxcompiler_LIBRARY)

message("Dxc libraries found: ${DXC_LIBRARIES}")

find_package_handle_standard_args(DXC REQUIRED_VARS DXC_INCLUDE_DIR DXC_LIBRARIES)

##################################
# Create targets
##################################

if(NOT CMAKE_VERSION VERSION_LESS 3.0 AND DXC_FOUND)
add_library(DXC::dxc SHARED IMPORTED)
set_target_properties(DXC::dxc PROPERTIES
          INTERFACE_INCLUDE_DIRECTORIES  ${DXC_INCLUDE_DIR}
          IMPORTED_IMPLIB  ${DXC_LIBRARIES}
          IMPORTED_LOCATION  ${DXC_LIBRARIES})
endif()