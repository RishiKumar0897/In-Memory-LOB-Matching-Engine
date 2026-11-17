# Core library shared by the app and tests.
add_library(lob core/order_book.cpp)
# PUBLIC: anything linking lob can also #include "lob/...".
target_include_directories(lob PUBLIC ${PROJECT_SOURCE_DIR}/include)
# PRIVATE: warning flags apply to lob's own files only.
target_link_libraries(lob PRIVATE lob_warnings)
