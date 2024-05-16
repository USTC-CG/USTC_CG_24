file(GLOB test_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(source ${test_sources})
    UCG_ADD_TEST(SRC ${source} LIBS GUI nodes RCore HD_USTC_CG_HWRT hd_USTC_CG_GL hd_USTC_CG vt Python3::Python EnTT::EnTT)
endforeach()