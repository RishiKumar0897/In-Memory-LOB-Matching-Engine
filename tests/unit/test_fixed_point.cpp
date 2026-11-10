#include "lob/common/fixed_point.h"
#include <gtest/gtest.h>

TEST(FixedPoint, ParsePrice) {
    lob::TickSize tick_size{1, 2}; // 2 decimal places, tick size of 0.01
    EXPECT_EQ(parse_price("123.45", 6, tick_size), 12345);
    EXPECT_EQ(parse_price("-123.45", 7, tick_size), -12345);
    EXPECT_EQ(parse_price("0.01", 4, tick_size), 1);
    EXPECT_EQ(parse_price("0.00", 4, tick_size), 0);
    EXPECT_EQ(parse_price("123", 3, tick_size), 12300); // no decimal part
    EXPECT_EQ(parse_price("123.456", 7, tick_size), 0); // too many decimal places
    EXPECT_EQ(parse_price("abc", 3, tick_size), 0); // invalid input
    EXPECT_EQ(parse_price("", 0, tick_size), 0); // empty input
}

