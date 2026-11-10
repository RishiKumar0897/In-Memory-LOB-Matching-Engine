add_library(lob core/order_book.cpp)
target_include_directories(lob PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(lob PRIVATE lob_warnings)
