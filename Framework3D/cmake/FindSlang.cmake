message("slang downloaded files at ${SLANG_ROOT_DIR}, trying to find ${Slang_FIND_COMPONENTS}")

include(FindPackageHandleStandardArgs)

# Define search paths based on user input and environment variables
set(SLANG_SEARCH_DIR ${SLANG_ROOT_DIR})
##################################
# Find the SLANG include dir
##################################
  
find_path(SLANG_INCLUDE_DIRS slang.h
    HINTS ${SLANG_INCLUDE_DIR} ${SLANG_SEARCH_DIR})

set(SLANG_SEARCH_COMPOMPONENTS slang slang-rt gfx)
set(SLANG_LIB_PATH_SUFFIX "bin/windows-x64/release")
# Find each component
foreach(_comp ${SLANG_SEARCH_COMPOMPONENTS})
if(";${Slang_FIND_COMPONENTS};slang;" MATCHES ";${_comp};")

    # Search for the libraries
    find_library(SLANG_${_comp}_LIBRARY ${_comp}
        HINTS ${SLANG_LIBRARY} ${SLANG_SEARCH_DIR}
        PATH_SUFFIXES ${SLANG_LIB_PATH_SUFFIX})

    if(SLANG_${_comp}_LIBRARY)
    list(APPEND SLANG_LIBRARIES "${SLANG_${_comp}_LIBRARY}")
    endif()

    if(SLANG_${_comp}_LIBRARY AND EXISTS "${SLANG_${_comp}_LIBRARY}")
    set(SLANG_${_comp}_FOUND TRUE)
    else()
    set(SLANG_${_comp}_FOUND FALSE)
    endif()

    # Mark internal variables as advanced
    mark_as_advanced(SLANG_${_comp}_LIBRARY)
endif()
endforeach()
message("Slang libraries found: ${SLANG_LIBRARIES}")

find_package_handle_standard_args(SLANG REQUIRED_VARS SLANG_INCLUDE_DIRS SLANG_LIBRARIES)

##################################
# Create targets
##################################

if(NOT CMAKE_VERSION VERSION_LESS 3.0 AND SLANG_FOUND)
add_library(SLANG::slang SHARED IMPORTED)
set_target_properties(SLANG::slang PROPERTIES
          INTERFACE_INCLUDE_DIRECTORIES  ${SLANG_INCLUDE_DIRS}
          IMPORTED_IMPLIB  ${SLANG_LIBRARIES}
          IMPORTED_LOCATION  ${SLANG_LIBRARIES})
endif()