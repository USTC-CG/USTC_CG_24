file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB NODE_FILES ${CMAKE_CURRENT_LIST_DIR}/Nodes/GNodes/*.cpp)

add_library(GCore STATIC ${CPP_FILES})

target_link_libraries(GCore PUBLIC OpenMeshCore)

target_include_directories(GCore PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(GCore PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_compile_options(GCore PRIVATE -DUSTC_CG_BUILD_MODULE=1 -DNOMINMAX)
set_target_properties(GCore PROPERTIES ${OUTPUT_DIR})

target_link_libraries(GCore 
	PUBLIC
	usdVol
	OpenMeshCore
	usdGeom 
	${TBB_tbb_LIBRARY}
)
target_compile_features(GCore PUBLIC cxx_std_20)