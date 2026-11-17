#include "lob/protocol/execution_report.h"
#include <gtest/gtest.h>

#include <cstddef>
#include <type_traits>

// Same idea as the inbound messages: fields go largest to smallest, so padding is only at the tail.
// If someone reorders a struct and adds padding, these sizes change and the test fails.
TEST(ExecutionReport, FieldOrdersAreCorrect) {
    // 8 (seq) + 8 (maker) + 8 (taker) + 8 (price) + 4 (qty) + 1 (side) = 37 -> 40 (align 8)
    EXPECT_EQ(sizeof(lob::Trade), 40u);
    // 8 (seq) + 8 (id) + 8 (price) + 3x4 (qtys) + 3x1 (enums) = 39 -> 40 (align 8)
    EXPECT_EQ(sizeof(lob::ExecutionReport), 40u);

    // Enum fields are packed together after the 4-byte fields.
    EXPECT_EQ(offsetof(lob::ExecutionReport, side) + 1, offsetof(lob::ExecutionReport, type));
    EXPECT_EQ(offsetof(lob::ExecutionReport, type) + 1, offsetof(lob::ExecutionReport, reason));
}

// Each enum is stored in 1 byte (the `: std::uint8_t`).
TEST(ExecutionReport, EnumsAreOneByte) {
    EXPECT_EQ(sizeof(lob::ExecType), 1u);
    EXPECT_EQ(sizeof(lob::RejectReason), 1u);
}

// Reports are plain data: cheap to copy into queues and safe to memcpy.
TEST(ExecutionReport, AreTriviallyCopyable) {
    EXPECT_TRUE(std::is_trivially_copyable_v<lob::Trade>);
    EXPECT_TRUE(std::is_trivially_copyable_v<lob::ExecutionReport>);
}

// `{}` zero-initialises every field, so a fresh report means "not rejected".
TEST(ExecutionReport, DefaultReasonIsNone) {
    EXPECT_EQ(static_cast<int>(lob::RejectReason::None), 0);

    lob::ExecutionReport report{};
    EXPECT_EQ(report.reason, lob::RejectReason::None);
    EXPECT_EQ(report.leavesQty, 0u);
    EXPECT_EQ(report.cumQty, 0u);
}

// A partial fill: an order for 100 trades 30, so 70 is left and 30 is filled in total.
// leaves + cum must always equal the original quantity while the order is live.
TEST(ExecutionReport, PartialFillQuantitiesAddUp) {
    lob::Order order{};
    order.id = 1;
    order.price = 100;
    order.originalQuantity = 100;
    order.remainingQuantity = 100;
    order.side = lob::Side::Buy;

    order.fill(30);

    lob::ExecutionReport report{.seq = 1,
                                .orderId = order.id,
                                .price = order.price,
                                .lastQty = 30,
                                .leavesQty = order.remainingQuantity,
                                .cumQty = order.cumulativeFilledQuantity,
                                .side = order.side,
                                .type = lob::ExecType::PartialFill,
                                .reason = lob::RejectReason::None};

    EXPECT_EQ(report.leavesQty, 70u);
    EXPECT_EQ(report.cumQty, 30u);
    EXPECT_EQ(report.leavesQty + report.cumQty, order.originalQuantity);
}

// A rejected order never traded: nothing filled, nothing left open, and a reason is set.
TEST(ExecutionReport, RejectCarriesReason) {
    lob::ExecutionReport report{.seq = 2,
                                .orderId = 5,
                                .price = 100,
                                .lastQty = 0,
                                .leavesQty = 0,
                                .cumQty = 0,
                                .side = lob::Side::Sell,
                                .type = lob::ExecType::Rejected,
                                .reason = lob::RejectReason::InvalidQuantity};

    EXPECT_EQ(report.type, lob::ExecType::Rejected);
    EXPECT_EQ(report.reason, lob::RejectReason::InvalidQuantity);
    EXPECT_EQ(report.leavesQty, 0u);
    EXPECT_EQ(report.cumQty, 0u);
}

// A trade records both sides of a match, priced at the resting (maker) order.
TEST(ExecutionReport, TradeHoldsBothSides) {
    lob::Trade trade{.seq = 3,
                     .makerId = 10,
                     .takerId = 20,
                     .price = 100,
                     .qty = 25,
                     .aggressorSide = lob::Side::Buy};

    EXPECT_EQ(trade.makerId, 10u);
    EXPECT_EQ(trade.takerId, 20u);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.qty, 25u);
    EXPECT_EQ(trade.aggressorSide, lob::Side::Buy);
}
