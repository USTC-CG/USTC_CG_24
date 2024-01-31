add_executable(5_map_Polynomial test.cpp)

target_include_directories(5_map_Polynomial PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(5_map_Polynomial PolynomialListLib PolynomialMapLib)

set_target_properties(5_map_Polynomial PROPERTIES ${OUTPUT_PROP})
