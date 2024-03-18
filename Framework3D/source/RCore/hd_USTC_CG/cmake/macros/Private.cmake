function(_classes LIBRARY_NAME)
    # Install headers to build or install prefix
    set(options PUBLIC PRIVATE)
    cmake_parse_arguments(classes
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    # If both get set, fall back to public.
    if(${classes_PUBLIC})
        set(VISIBILITY "PUBLIC")
    elseif(${classes_PRIVATE})
        set(VISIBILITY "PRIVATE")
    else()
        message(FATAL_ERROR
            "Library ${LIBRARY_NAME} has implicit visibility.  "
            "Provide PUBLIC or PRIVATE to classes() call.")
    endif()

    # Should the classes have an argument name?
    foreach(cls ${classes_UNPARSED_ARGUMENTS})
        list(APPEND ${LIBRARY_NAME}_${VISIBILITY}_HEADERS ${cls}.h)
        list(APPEND ${LIBRARY_NAME}_CPPFILES ${cls}.cpp)
    endforeach()
    set(${LIBRARY_NAME}_${VISIBILITY}_HEADERS
        ${${LIBRARY_NAME}_${VISIBILITY}_HEADERS}
        PARENT_SCOPE
    )
    set(${LIBRARY_NAME}_CPPFILES ${${LIBRARY_NAME}_CPPFILES} PARENT_SCOPE)
endfunction() # _classes

function(_copy_headers LIBRARY_NAME)
    set(options  "")
    set(oneValueArgs PREFIX)
    set(multiValueArgs FILES)
    cmake_parse_arguments(_args
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    set(files_copied "")
    set(hpath "${_args_PREFIX}/${LIBRARY_NAME}")
    if ("${CMAKE_CURRENT_SOURCE_DIR}" MATCHES ".*/pxr/.*")
        # Include paths under pxr/ match the source path.
        file(RELATIVE_PATH hpath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    set(header_dest_dir "${PROJECT_BINARY_DIR}/${PXR_INSTALL_SUBDIR}/include/${hpath}")
    if( NOT "${_args_FILES}" STREQUAL "")
        set(files_copied "")
        foreach (f ${_args_FILES})
            set(infile "${CMAKE_CURRENT_SOURCE_DIR}/${f}")
            set(outfile "${header_dest_dir}/${f}")
            get_filename_component(dir_to_create "${outfile}" PATH)
            add_custom_command(
                OUTPUT ${outfile}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${dir_to_create}
                COMMAND ${CMAKE_COMMAND} -Dinfile=${infile} -Doutfile=${outfile} -P "${CMAKE_CURRENT_LIST_DIR}/cmake/macros/copyHeaderForBuild.cmake"
                MAIN_DEPENDENCY ${infile}
                COMMENT "Copying ${f} ..."
                VERBATIM
            )
            list(APPEND files_copied ${outfile})
        endforeach()
    endif()

    # Add a headers target.
    add_custom_target(${LIBRARY_NAME}_headerfiles
        DEPENDS ${files_copied}
    )
    set_target_properties(${LIBRARY_NAME}_headerfiles
        PROPERTIES
            FOLDER "headerfiles"
    )

    # Make sure headers are installed before building the library.
    add_dependencies(${LIBRARY_NAME} ${LIBRARY_NAME}_headerfiles)
endfunction() # _copy_headers

function(_plugInfo_subst libTarget pluginInstallPrefix pluginToLibraryPath plugInfoPath)
    _get_resources_dir_name(PLUG_INFO_RESOURCE_PATH)
    set(PLUG_INFO_ROOT "..")
    set(PLUG_INFO_PLUGIN_NAME "pxr.${libTarget}")
    set(PLUG_INFO_LIBRARY_PATH "${pluginToLibraryPath}")

    message("prefix: ${pluginInstallPrefix}")

    configure_file(
        ${plugInfoPath}
        ${OUT_BINARY_DIR}/${pluginInstallPrefix}/${libTarget}/resources/${plugInfoPath}
    )
endfunction() # _plugInfo_subst

function(_get_resources_dir_name output)
    set(${output}
        resources
        PARENT_SCOPE)
endfunction() # _get_resources_dir_name

function(_get_resources_dir pluginsPrefix pluginName output)
    _get_resources_dir_name(resourcesDir)
    set(${output}
        ${pluginsPrefix}/${pluginName}/${resourcesDir}
        PARENT_SCOPE)
endfunction() # _get_resources_dir

function(_copy_resource_files NAME pluginInstallPrefix pluginToLibraryPath)
    # Resource files install into a structure that looks like:
    # lib/
    #     usd/
    #         ${NAME}/
    #             resources/
    #                 resourceFileA
    #                 subdir/
    #                     resourceFileB
    #                     resourceFileC
    #                 ...
    #
    _get_resources_dir(${pluginInstallPrefix} ${NAME} resourcesPath)

    foreach(resourceFile ${ARGN})
        # A resource file may be marked to not do any variable substitution,
        # like <src file>:no_subst, for these resources _plugInfo_subst will not
        # be called
        # A resource file may be specified like <src file>:<dst file> to
        # indicate that it should be installed to a different location in
        # the resources area. Check if this is the case.
        string(REPLACE ":" ";" resourceFile "${resourceFile}")
        list(LENGTH resourceFile n)
        if (n EQUAL 1)
           set(resourceDestFile ${resourceFile})
        elseif (n EQUAL 2)
           list(GET resourceFile 1 secondaryOption)
           list(GET resourceFile 0 resourceFile)
           if (${secondaryOption} STREQUAL "no_subst")
               set(plugInfoNoSubstitution ON)
               set(resourceDestFile ${resourceFile})
           else()
               # secondaryOption provides resourceDestFile
               set(resourceDestFile ${secondaryOption})
           endif()
        else()
           message(FATAL_ERROR
               "Failed to parse resource path ${resourceFile}")
        endif()

        get_filename_component(dirPath ${resourceDestFile} PATH)
        get_filename_component(destFileName ${resourceDestFile} NAME)

        # plugInfo.json go through an initial template substitution step files
        # install it from the binary (gen) directory specified by the full
        # path. Otherwise, use the original relative path which is relative to
        # the source directory.
        if (${destFileName} STREQUAL "plugInfo.json")
            if (DEFINED plugInfoNoSubstitution)
                # Do not substitute variables and only copy the plugInfo file
                configure_file(
                    ${resourceFile}
                    ${OUT_BINARY_DIR}/${pluginInstallPrefix}/${NAME}/resources/${resourceFile}
                    COPYONLY
                )
            else()
                _plugInfo_subst(${NAME} "${pluginInstallPrefix}" "${pluginToLibraryPath}" ${resourceFile})
            endif()
            set(resourceFile "${CMAKE_CURRENT_BINARY_DIR}/${resourceFile}")
        endif()

        install(
            FILES ${resourceFile}
            DESTINATION ${resourcesPath}/${dirPath}
            RENAME ${destFileName}
        )
    endforeach()
endfunction() # _copy_resource_files

# Add a library target named NAME.
function(_pxr_library NAME)
    # Argument parsing.
    set(options
    )
    set(oneValueArgs
        PREFIX
        SUBDIR
        SUFFIX
        TYPE
        PRECOMPILED_HEADERS
        PRECOMPILED_HEADER_NAME
    )
    set(multiValueArgs
        PUBLIC_HEADERS
        PRIVATE_HEADERS
        CPPFILES
        LIBRARIES
        INCLUDE_DIRS
        RESOURCE_FILES
        DOXYGEN_FILES
        LIB_INSTALL_PREFIX_RESULT
    )
    cmake_parse_arguments(args
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    
    if(args_TYPE STREQUAL "STATIC")
        # Building an explicitly static library.
        add_library(${NAME}
            STATIC
            ${args_CPPFILES}
            ${args_PUBLIC_HEADERS}
            ${args_PRIVATE_HEADERS}
        )

    else()
        # Building an explicitly shared library or plugin.
        add_library(${NAME}
            SHARED
            ${args_CPPFILES}
            ${args_PUBLIC_HEADERS}
            ${args_PRIVATE_HEADERS}
        )
    endif()
    message("${NAME} links to ${args_LIBRARIES}")
    target_link_libraries(${NAME} PUBLIC ${args_LIBRARIES})

    message("Public headers: ${args_PUBLIC_HEADERS}")
    # Copy headers to the build directory and include from there and from
    # external packages.
    _copy_headers(${NAME}
        FILES
            ${args_PUBLIC_HEADERS}
            ${args_PRIVATE_HEADERS}
        PREFIX
            ${PXR_PREFIX}
    )

    target_include_directories(${NAME}
    PRIVATE
        "${PROJECT_BINARY_DIR}/include"
        "${PROJECT_BINARY_DIR}/${PXR_INSTALL_SUBDIR}/include"
    INTERFACE
        $<INSTALL_INTERFACE:${headerInstallDir}>
    )

    set_target_properties(${NAME} PROPERTIES ${OUTPUT_DIR})

    set(libraryFilename "${args_PREFIX}${NAME}${args_SUFFIX}")

    set(pluginInstallPrefix "usd")
    set(libInstallPrefix "")

    set(pluginToLibraryPath "")

    # Figure out the relative path from this library's plugin location
    # (in the libplug sense, which applies even to non-plugins, and is
    # where we can find external resources for the library) to the
    # library's location.  This can be embedded into resource files.
    #
    # If we're building a monolithic library or individual static libraries,
    # these libraries are not separately loadable at runtime. In these cases,
    # we don't need to specify the library's location, so we leave
    # pluginToLibraryPath empty.
    if(NOT args_TYPE STREQUAL "STATIC")
        if(NOT (";${PXR_CORE_LIBS};" MATCHES ";${NAME};" AND _building_monolithic))
            file(RELATIVE_PATH
                pluginToLibraryPath
                ${CMAKE_INSTALL_PREFIX}/${pluginInstallPrefix}/${NAME}
                ${CMAKE_INSTALL_PREFIX}/${libInstallPrefix}/${libraryFilename})
        endif()
    endif()

    # Install resources for the NAME library, at appropriate paths
    _copy_resource_files(
        ${NAME}
        "${pluginInstallPrefix}"
        "${pluginToLibraryPath}"
        ${args_RESOURCE_FILES})

    target_compile_definitions(${NAME} PUBLIC NOMINMAX=1)
    string(TOUPPER ${NAME} CAPITAL_NAME)
    target_compile_definitions(${NAME} PRIVATE ${CAPITAL_NAME}_EXPORTS=1)
    target_include_directories(${NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

endfunction()