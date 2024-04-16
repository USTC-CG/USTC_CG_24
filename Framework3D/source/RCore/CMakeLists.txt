file(GLOB_RECURSE internal_files ${CMAKE_CURRENT_LIST_DIR}/internal/*.cpp)
set(CPP_FILES 
	ROP.cpp
	${internal_files}
	hd_USTC_CG_GL/light.cpp
	hd_USTC_CG_GL/material.cpp
	hd_USTC_CG_GL/camera.cpp
	hd_USTC_CG_GL/geometries/mesh.cpp
)

# Anything related to imgui should be set to STATIC
add_library(RCore STATIC ${CPP_FILES})

target_link_libraries(RCore PUBLIC OpenMeshCore)

target_include_directories(RCore PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(RCore PRIVATE -DUSTC_CG_BUILD_MODULE=1 -DNOMINMAX)
set_target_properties(RCore PROPERTIES ${OUTPUT_DIR})

target_link_libraries(RCore PUBLIC usdGeom nodes)
target_compile_features(RCore PUBLIC cxx_std_20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/macros)
include(pxrPlugin)

add_subdirectory(hd_USTC_CG)
target_compile_options(hd_USTC_CG PRIVATE -DUSTC_CG_BUILD_MODULE=1)

add_subdirectory(hd_USTC_CG_GL)
target_compile_options(hd_USTC_CG_GL PRIVATE -DUSTC_CG_BUILD_MODULE=1)

