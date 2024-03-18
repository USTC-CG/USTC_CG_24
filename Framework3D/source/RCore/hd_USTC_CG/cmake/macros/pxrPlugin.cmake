include(Private)

macro(pxr_plugin NAME)
    pxr_library(${NAME} TYPE "PLUGIN" ${ARGN})
endmacro(pxr_plugin)

function(pxr_library NAME)
    set(options
        DISABLE_PRECOMPILED_HEADERS
    )
    set(oneValueArgs
        TYPE
        PRECOMPILED_HEADER_NAME
    )
    set(multiValueArgs
        PUBLIC_CLASSES
        PUBLIC_HEADERS
        PRIVATE_CLASSES
        PRIVATE_HEADERS
        CPPFILES
        LIBRARIES
        INCLUDE_DIRS
        DOXYGEN_FILES
        RESOURCE_FILES
        PYTHON_PUBLIC_CLASSES
        PYTHON_PRIVATE_CLASSES
        PYTHON_PUBLIC_HEADERS
        PYTHON_PRIVATE_HEADERS
        PYTHON_CPPFILES
        PYMODULE_CPPFILES
        PYMODULE_FILES
        PYSIDE_UI_FILES
    )

    cmake_parse_arguments(args
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    # Expand classes into filenames.
    _classes(${NAME} ${args_PRIVATE_CLASSES} PRIVATE)
    _classes(${NAME} ${args_PUBLIC_CLASSES} PUBLIC)

    set(CPPFILES "${args_CPPFILES};${${NAME}_CPPFILES}")
    
    if(args_TYPE STREQUAL "STATIC")
        set(suffix ${CMAKE_STATIC_LIBRARY_SUFFIX})
    else()
        set(suffix ${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif()
    _pxr_library(${NAME}
        TYPE "${args_TYPE}"
        PREFIX "${prefix}"
        SUFFIX "${suffix}"
        SUBDIR "${subdir}"
        CPPFILES "${args_CPPFILES};${${NAME}_CPPFILES}"
        PUBLIC_HEADERS "${args_PUBLIC_HEADERS};${${NAME}_PUBLIC_HEADERS}"
        PRIVATE_HEADERS "${args_PRIVATE_HEADERS};${${NAME}_PRIVATE_HEADERS}"
        LIBRARIES "${args_LIBRARIES}"
        INCLUDE_DIRS "${args_INCLUDE_DIRS}"
        RESOURCE_FILES "${args_RESOURCE_FILES}"
        DOXYGEN_FILES "${args_DOXYGEN_FILES}"
        PRECOMPILED_HEADERS "${pch}"
        PRECOMPILED_HEADER_NAME "${args_PRECOMPILED_HEADER_NAME}"
        LIB_INSTALL_PREFIX_RESULT libInstallPrefix
    )
endfunction() #pxr_library