add_library(PolynomialMapLib STATIC ./PolynomialMap.cpp)

target_include_directories(PolynomialMapLib PUBLIC
  ${PROJECT_SOURCE_DIR}/include)

set_target_properties(PolynomialMapLib PROPERTIES ${OUTPUT_PROP})
