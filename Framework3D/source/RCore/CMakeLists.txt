set(CPP_FILES 
	ROP.cpp
)

# Anything related to imgui should be set to STATIC
add_library(RCore STATIC ${CPP_FILES})

target_link_libraries(RCore PUBLIC OpenMeshCore)

target_include_directories(RCore PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(RCore PRIVATE -DUSTC_CG_BUILD_MODULE=1)
set_target_properties(RCore PROPERTIES ${OUTPUT_DIR})

target_link_libraries(RCore PUBLIC usdGeom)
target_compile_features(RCore PUBLIC cxx_std_20)

add_subdirectory(hd_USTC_CG)

target_link_libraries(RCore PRIVATE hd_USTC_CG)