set(PXR_PREFIX pxr/imaging)
set(PXR_PACKAGE HD_USTC_CG_HWRT)

message("hd_USTC_CG_HWRT at ${CMAKE_CURRENT_LIST_DIR}")

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
get_relative_paths(optix optixHeaders)

pxr_plugin(${PXR_PACKAGE}
   LIBRARIES
        plug
        tf
        vt
        gf
        work
        hf
        hd
        hdx
        ${TBB_tbb_LIBRARY}
        embree
        usdLux
        RCore
        usdImaging

    INCLUDE_DIRS
        ${TBB_INCLUDE_DIRS}

    PUBLIC_CLASSES
        renderer
        renderDelegate
        rendererPlugin
        renderPass
        renderBuffer
        config
        sampler
        instancer
        integrator
        material
        camera
        light
        texture
        integrators/ao
        integrators/direct
        integrators/path

        geometries/mesh
        geometries/meshSamplers

    RESOURCE_FILES
        plugInfo.json
        ${RelativeShaderPaths}
        ${optixHeaders}
)

target_include_directories(${PXR_PACKAGE} PRIVATE ${CMAKE_CURRENT_LIST_DIR})