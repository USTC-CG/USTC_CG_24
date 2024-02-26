project(view)
file(GLOB source
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${INCLUDE_DIR}/view/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/shapes/*.cpp"
  "${INCLUDE_DIR}/view/shapes/*.h"
)
add_library(${PROJECT_NAME} ${source})
set_target_properties(${PROJECT_NAME} PROPERTIES ${OUTPUT_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw glad imgui)
target_include_directories(${PROJECT_NAME} 
  PUBLIC ${INCLUDE_DIR} 
  PUBLIC ${THIRD_PARTY_DIR}
)