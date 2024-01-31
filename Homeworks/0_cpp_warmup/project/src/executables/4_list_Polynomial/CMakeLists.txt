add_executable(4_list_Polynomial main.cpp)

target_include_directories(4_list_Polynomial PUBLIC ${PROJECT_SOURCE_DIR}/include)

# find_library(lib PolynomialListLib PATHS ${PROJECT_SOURCE_DIR}/src/libraries/shared)
add_library(PolynomialListLib SHARED ${PROJECT_SOURCE_DIR}/src/libraries/shared/PolynomialList.cpp)

target_include_directories(PolynomialListLib PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(4_list_Polynomial PolynomialListLib)