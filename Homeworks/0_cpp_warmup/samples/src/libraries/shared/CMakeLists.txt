add_library(PolynomialListLib SHARED ./PolynomialList.cpp)

target_include_directories(PolynomialListLib PUBLIC
  ${PROJECT_SOURCE_DIR}/include)