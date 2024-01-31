add_executable(5_map_Polynomial test.cpp)

target_include_directories(5_map_Polynomial PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(PolynomialMapLib STATIC ${PROJECT_SOURCE_DIR}/src/libraries/static/PolynomialMap.cpp)

target_include_directories(PolynomialMapLib PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(5_map_Polynomial PolynomialListLib PolynomialMapLib)