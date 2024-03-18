set(PXR_PREFIX pxr/imaging)
set(PXR_PACKAGE hd_USTC_CG)

list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}/cmake/defaults
    ${CMAKE_CURRENT_LIST_DIR}/cmake/modules
    ${CMAKE_CURRENT_LIST_DIR}/cmake/macros)

message("hd_USTC_CG at ${CMAKE_CURRENT_LIST_DIR}")

include(pxrPlugin)

add_custom_target(shared_libs)

message("TBB_tbb_LIBRARY: ${TBB_tbb_LIBRARY}")

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

    INCLUDE_DIRS
        ${TBB_INCLUDE_DIRS}

    PUBLIC_CLASSES
        renderer
        rendererEmbree
        renderDelegate
        rendererPlugin
        renderPass
        renderBuffer
        config
        sampler
        instancer
        integrator
        camera
        light

        integrators/ao
        integrators/direct

        geometries/mesh
        geometries/meshSamplers

    RESOURCE_FILES
        plugInfo.json
)

target_include_directories(${PXR_PACKAGE} PRIVATE ${CMAKE_CURRENT_LIST_DIR})