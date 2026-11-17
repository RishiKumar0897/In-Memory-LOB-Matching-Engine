// Core types shared across the order book.
// An order book holds bids (buys, highest price first) and asks (sells, lowest price first);
// the matching engine trades them when prices cross.
#pragma once // include this file only once per compilation

#include <cassert>
#include <cstdint>

#include "lob/common/time.h"

namespace lob {
// Aliases: clearer names for plain integer types.
using OrderId = std::uint64_t;
using Price = std::int64_t;     // whole number of ticks, not dollars (exact, unlike double)
using Quantity = std::uint32_t; // shares/contracts

// enum class = scoped named constants; `: std::uint8_t` stores each in 1 byte.
enum class Side : std::uint8_t { Buy, Sell };
enum class OrderType : std::uint8_t { Limit, Market }; // Limit = at this price or better; Market = any price, now
enum class TimeInForce : std::uint8_t { GTC, IOC, FOK }; // Good 'til Cancelled, Immediate or Cancel, Fill or Kill
using TraderId = std::uint32_t;
using SymbolId = std::uint32_t; // instrument, as a number (cheaper than a string)

// One order. Fields go largest to smallest to minimise padding bytes.
struct Order {
    OrderId id;
    Price price;         // ignored for Market orders
    Timestamp timestamp; // arrival time; breaks ties at the same price (earlier fills first)
    Quantity originalQuantity;
    Quantity remainingQuantity;
    Quantity cumulativeFilledQuantity;
    TraderId traderId;
    SymbolId symbolId;
    Side side;
    OrderType orderType;
    TimeInForce timeInForce;

    // Record that q units traded. asserts run in Debug only.
    void fill(Quantity q) noexcept {
        assert(q <= remainingQuantity);               // can't trade more than is left
        remainingQuantity        -= q;
        cumulativeFilledQuantity += q;
        assert(originalQuantity == remainingQuantity + cumulativeFilledQuantity); // must always hold
    }

    bool isFilled() const noexcept { return remainingQuantity == 0; }
};

} // namespace lob
