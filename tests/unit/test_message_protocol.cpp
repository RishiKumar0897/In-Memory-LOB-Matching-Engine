#include "lob/protocol/messages.h"
#include <gtest/gtest.h>

#include <cstddef>
#include <type_traits>
#include <variant>

// Fields are ordered largest to smallest, so the only padding is at the tail.
// If someone reorders a struct and adds padding, these sizes change and the test fails.
TEST(MessageTypes, FieldOrdersAreCorrect) {
    // 8 (id) + 8 (price) + 4 (qty) + 4 (symbol) + 4 (trader) + 3x1 (enums) = 31 -> 32 (align 8)
    EXPECT_EQ(sizeof(lob::NewOrder), 32u);
    // 8 (id)
    EXPECT_EQ(sizeof(lob::CancelOrder), 8u);
    // 8 (id) + 8 (price) + 4 (qty) = 20 -> 24 (align 8)
    EXPECT_EQ(sizeof(lob::ModifyOrder), 24u);

    // Enum fields are packed together after the 4-byte fields.
    EXPECT_EQ(offsetof(lob::NewOrder, side) + 1, offsetof(lob::NewOrder, type));
    EXPECT_EQ(offsetof(lob::NewOrder, type) + 1, offsetof(lob::NewOrder, tif));
}

// Messages are plain data: cheap to copy into queues and safe to memcpy.
TEST(MessageTypes, AreTriviallyCopyable) {
    EXPECT_TRUE(std::is_trivially_copyable_v<lob::NewOrder>);
    EXPECT_TRUE(std::is_trivially_copyable_v<lob::CancelOrder>);
    EXPECT_TRUE(std::is_trivially_copyable_v<lob::ModifyOrder>);
}

TEST(MessageTypes, CommandHoldsEachMessage) {
    lob::Command cmd = lob::NewOrder{.client_order_id = 1,
                                     .price = 100,
                                     .qty = 10,
                                     .symbolId = 7,
                                     .traderId = 3,
                                     .side = lob::Side::Buy,
                                     .type = lob::OrderType::Limit,
                                     .tif = lob::TimeInForce::GTC};
    ASSERT_TRUE(std::holds_alternative<lob::NewOrder>(cmd));
    EXPECT_EQ(std::get<lob::NewOrder>(cmd).price, 100);
    EXPECT_EQ(std::get<lob::NewOrder>(cmd).side, lob::Side::Buy);

    cmd = lob::CancelOrder{.orderId = 42};
    ASSERT_TRUE(std::holds_alternative<lob::CancelOrder>(cmd));
    EXPECT_EQ(std::get<lob::CancelOrder>(cmd).orderId, 42u);

    cmd = lob::ModifyOrder{.orderId = 42, .newPrice = 105, .newQty = 5};
    ASSERT_TRUE(std::holds_alternative<lob::ModifyOrder>(cmd));
    EXPECT_EQ(std::get<lob::ModifyOrder>(cmd).newPrice, 105);
    EXPECT_EQ(std::get<lob::ModifyOrder>(cmd).newQty, 5u);
}
