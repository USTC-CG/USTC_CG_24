list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/macros)
include(pxrPlugin)


set(PXR_PREFIX pxr/usd)
set(PXR_PACKAGE usdFoam)

message("usdFoam at ${CMAKE_CURRENT_LIST_DIR}")
# Plugin configuration
pxr_plugin(${PXR_PACKAGE}
    LIBRARIES
        plug
        tf
        vt
        gf
        usdGeom
        usd
        ${TBB_tbb_LIBRARY}
    
    INCLUDE_DIRS
        ${TBB_INCLUDE_DIRS}

    PUBLIC_CLASSES
        plateauBorder
        polygonFilm
        root
        sphereFilm
        tokens

    PUBLIC_HEADERS
        api.h

    RESOURCE_FILES
        plugInfo.json
        schema.usda
        generatedSchema.usda
)

target_include_directories(${PXR_PACKAGE} PRIVATE ${CMAKE_CURRENT_LIST_DIR})