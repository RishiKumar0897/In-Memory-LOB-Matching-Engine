// Outbound messages (fills, acks, rejects) the engine sends back.
#pragma once

#include <cstdint>
#include "lob/common/types.h"

namespace lob {

    // What happened to an order in this report.
    enum class ExecType : std::uint8_t {
        New,         // accepted and resting on the book (an "ack")
        PartialFill, // some traded, some still open
        Fill,        // fully traded
        Canceled,
        Replaced,    // modify succeeded
        Rejected,    // see RejectReason
    };

    // One match between two orders.
    // Maker = order already resting on the book; taker = incoming order that crossed it.
    struct Trade {
        std::uint64_t seq; // engine sequence number, for ordering events
        OrderId makerId;
        OrderId takerId;
        Price price;       // trades happen at the maker's price
        Quantity qty;
        Side aggressorSide; // the taker's side
    };

    // Why an order was rejected. None = not rejected.
    enum class RejectReason : std::uint8_t {
        None = 0,

        // Validation
        InvalidQuantity,      // e.g. zero
        InvalidPrice,         // e.g. not on the tick grid
        UnknownInstrument,
        UnsupportedOrderType,
        DuplicateOrderId,

        // Market State
        MarketClosed,
        SymbolHalted,         // trading paused for this symbol

        // Execution Constraints
        PostOnlyWouldTake,    // post-only order would have traded instead of resting
        FokCannotFill,        // Fill-or-Kill couldn't fill in full
        NoLiquidity,          // market order with nothing to trade against
        SelfTradePrevention,  // would have traded against the same trader
        PriceBandViolation,   // price too far from the market

        // Cancel / Modify
        OrderNotFound,
        InvalidModify,
    };

    // Status update sent to the trader after each event on their order.
    struct ExecutionReport {
    std::uint64_t seq;       // engine sequence number
    OrderId orderId;
    Price price;             // last fill price (Fill/PartialFill), else order price
    Quantity lastQty;        // qty filled in this event (0 if not a fill)
    Quantity leavesQty;      // still open on the book
    Quantity cumQty;         // total filled so far
    Side side;
    ExecType type;
    RejectReason reason;     // None unless type == Rejected
    };

} //namespace lob
