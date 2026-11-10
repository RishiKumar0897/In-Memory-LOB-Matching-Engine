add_executable(lob_tests unit/test_order_book.cpp)
target_link_libraries(lob_tests PRIVATE lob lob_warnings GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(lob_tests)
