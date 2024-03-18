set(CPP_FILES 
	SOP.cpp
	SequencedComponent.cpp
)
add_library(SCore ${CPP_FILES})

target_link_libraries(SCore PUBLIC GCore)

target_include_directories(SCore PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(SCore 
	PRIVATE 
	-DUSTC_CG_BUILD_MODULE=1
	-DNOMINMAX
)
set_target_properties(SCore PROPERTIES ${OUTPUT_DIR})

target_link_libraries(SCore PUBLIC 
	usdGeom 
	usdVol 
	hioOpenVDB 
	${TBB_tbb_LIBRARY}
)
target_compile_features(SCore PUBLIC cxx_std_20)