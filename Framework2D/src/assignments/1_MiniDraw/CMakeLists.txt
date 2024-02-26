project(1_MiniDraw)
file(GLOB source
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h" 
)
add_executable(${PROJECT_NAME} ${source})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES ${OUTPUT_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC view) 