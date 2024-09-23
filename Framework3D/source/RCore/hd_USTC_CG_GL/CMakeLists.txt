set(PXR_PREFIX pxr/imaging)
set(PXR_PACKAGE hd_USTC_CG_GL)

message("hd_USTC_CG at ${CMAKE_CURRENT_LIST_DIR}")


# Define a function to generate a list of relative paths to files within a specified folder
function(get_relative_paths folder output_list)
    file(GLOB_RECURSE files ${folder}/*)
    set(local_list "")
    foreach(file ${files})
        get_filename_component(relativePath ${file} DIRECTORY)
        file(RELATIVE_PATH relativePathToCurrentDir ${CMAKE_CURRENT_SOURCE_DIR} ${relativePath})
        get_filename_component(fileName ${file} NAME)
        list(APPEND local_list "${relativePathToCurrentDir}/${fileName}")
    endforeach()
    set(${output_list} "${local_list}" PARENT_SCOPE)
endfunction()

get_relative_paths(shaders RelativeShaderPaths)
message("shit ${RelativeShaderPaths}")
get_relative_paths(optix optixHeaders)
get_relative_paths(nvapi nvapiHeaders)

pxr_plugin(${PXR_PACKAGE}
   LIBRARIES
        Python3::Python
        plug
        tf
        vt
        gf
        work
        hf
        hd
        hdx
        ${TBB_tbb_LIBRARY}
        usdLux
        nodes
        RCore
        usdImaging
        d3d12
        dxgi 
        DXC::dxc

    INCLUDE_DIRS
        ${TBB_INCLUDE_DIRS}

    PUBLIC_CLASSES
        renderer
        renderDelegate
        rendererPlugin
        renderPass
        renderBuffer
        renderTLAS
        config
        sampler
        instancer
        camera
        light
        material

        geometries/mesh

    RESOURCE_FILES
        plugInfo.json
        ${RelativeShaderPaths}
        ${optixHeaders}
        ${nvapiHeaders}
)

target_include_directories(${PXR_PACKAGE} PRIVATE ${CMAKE_CURRENT_LIST_DIR})