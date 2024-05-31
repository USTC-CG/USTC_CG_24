file(GLOB_RECURSE test_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(source ${test_sources})
    UCG_ADD_TEST(SRC ${source} LIBS RCore Python3::Python hd_USTC_CG_GL SLANG::slang)
endforeach()
target_link_libraries(hd_USTC_CG_test 
    PUBLIC 
    hd_USTC_CG
)