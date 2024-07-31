include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/releases/download/v1.15.0/googletest-1.15.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP True
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

set(TESTS
  test_combined
  test_lexer
  test_parser
  test_regex
)
foreach(TEST ${TESTS})
  add_executable("${TEST}" "${TEST}.cpp")
  target_link_libraries("${TEST}" PRIVATE GTest::gtest_main)
  target_link_libraries("${TEST}" PRIVATE lexer parser)
  target_include_directories("${TEST}" PUBLIC ../include)
  gtest_discover_tests("${TEST}")
endforeach()
