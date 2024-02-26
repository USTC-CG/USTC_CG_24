project(demo_hello_world)
file(GLOB source
  "${CMAKE_CURRENT_SOURCE_DIR}/demo_hello_world.cpp"
)
add_executable(${PROJECT_NAME} ${source})
set_target_properties(${PROJECT_NAME} PROPERTIES ${OUTPUT_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC view) 

project(demo)
file(GLOB source
  "${CMAKE_CURRENT_SOURCE_DIR}/demo.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/window_demo.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/window_demo.h"
)
add_executable(${PROJECT_NAME} ${source})
set_target_properties(${PROJECT_NAME} PROPERTIES ${OUTPUT_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC view) 