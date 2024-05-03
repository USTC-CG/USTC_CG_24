file(GLOB_RECURSE GUI_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(GUI STATIC ${GUI_source})

target_link_libraries(GUI PUBLIC 
	imgui 
	stb_image 
	GCore
	nodes
	RCore
	usdImaging
	usdImagingGL
)
target_compile_options(GUI PRIVATE -DUSTC_CG_BUILD_MODULE=0  -DNOMINMAX)

target_include_directories(GUI PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(GUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui-node-editor)

set_target_properties(GUI PROPERTIES ${OUTPUT_DIR})
