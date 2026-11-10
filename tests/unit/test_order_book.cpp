#include "lob/core/order_book.h"

#include <gtest/gtest.h>

TEST(OrderBook, StartsEmpty) {
    lob::OrderBook book;
    EXPECT_EQ(book.order_count(), 0u);

    // int* p = new int[1]; p[5] = 1; // This is a memory error, but we want to see if the test
    // framework catches it
}