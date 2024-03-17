project(2_ImageWarping)
file(GLOB source
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h" 
)
add_executable(${PROJECT_NAME} ${source})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES 
  DEBUG_POSTFIX "_d"
  RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DIR}"
  ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_DIR}") 
target_link_libraries(${PROJECT_NAME} PUBLIC view) 
target_compile_definitions(${PROJECT_NAME} PRIVATE -DDATA_PATH="${FRAMEWORK2D_DIR}/../Homeworks/2_image_warping/data")

#for libtorch
# project(2_ImageWarping)
# set(CMAKE_PREFIX_PATH "D:/C++lib/Libtorch/files/libtorch")
# find_package(Torch REQUIRED)
# file(GLOB source
#   "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/*.h" 
# )
# add_executable(${PROJECT_NAME} ${source})
# target_link_libraries(${PROJECT_NAME}  "${TORCH_LIBRARIES}"  )
# target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_include_directories(${PROJECT_NAME} PUBLIC "D:/C++lib/Libtorch/files/libtorch")
# set_target_properties(${PROJECT_NAME} PROPERTIES 
#   DEBUG_POSTFIX "_d"
#   RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}"
#   LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DIR}"
#   ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_DIR}") 
# target_link_libraries(${PROJECT_NAME} PUBLIC view) 
# target_compile_definitions(${PROJECT_NAME} PRIVATE -DDATA_PATH="${FRAMEWORK2D_DIR}/../Homeworks/2_image_warping/data")
# set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)```
