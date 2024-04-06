set(PXR_PREFIX pxr/imaging)
set(PXR_PACKAGE hd_USTC_CG_GL)

message("hd_USTC_CG at ${CMAKE_CURRENT_LIST_DIR}")

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
        usdLux
        nodes
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
        camera
        light
        material

        geometries/mesh

    RESOURCE_FILES
        plugInfo.json
)

target_include_directories(${PXR_PACKAGE} PRIVATE ${CMAKE_CURRENT_LIST_DIR})