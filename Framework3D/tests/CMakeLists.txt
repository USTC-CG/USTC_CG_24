function(UCG_ADD_TEST)
    set(options SHARED)
    set(oneValueArgs SRC)
    set(multiValueArgs LIBS LIB_FLAGS EXTRA_FILES)
    cmake_parse_arguments(UCG_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    string(REGEX REPLACE "(.*/)([a-zA-Z0-9_ ]+)(\.cpp|\.cu)" "\\2" test_name ${UCG_TEST_SRC}) 
    message("---- Adding the test ${test_name}.")

    add_executable(${test_name}_test ${UCG_TEST_SRC})

    set_target_properties(${test_name}_test PROPERTIES ${OUTPUT_DIR})

    # There should be googletest available
    target_link_libraries(${test_name}_test PUBLIC GTest::gtest GTest::gtest_main)
    target_link_libraries(${test_name}_test PUBLIC ${UCG_TEST_LIBS})
    target_include_directories(${test_name}_test PUBLIC ${PROJECT_SOURCE_DIR}/source)
    target_compile_definitions(${test_name}_test PUBLIC NOMINMAX=1)

    add_test(
    NAME
      ${test_name}_test
    COMMAND
      ${test_name}_test
  )
endfunction(UCG_ADD_TEST)

add_subdirectory(GCore)
add_subdirectory(SCore)
add_subdirectory(RCore)
add_subdirectory(GUI)
add_subdirectory(nodes)