# 检查是否是 Windows 平台
if(WIN32)
    # 定义 _WIN32 和 AsShared_PolynomialList 宏
    add_definitions(-D_WIN32)
    add_definitions(-DAsShared_PolynomialList)

    # 如果你正在编译 PolynomialList 库，并且希望导出它的符号
    add_definitions(-DExport_PolynomialList)
endif()

add_library(PolynomialListLib SHARED ./PolynomialList.cpp)

target_include_directories(PolynomialListLib PUBLIC
  ${PROJECT_SOURCE_DIR}/include)

set_target_properties(PolynomialListLib PROPERTIES ${OUTPUT_PROP})
