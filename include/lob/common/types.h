#pragma once

#include <cassert>
#include <cstdint>
#include "lob/common/time.h"

namespace lob {
using OrderId = std::uint64_t;
using Price = std::int64_t;
using Quantity = std::uint32_t;

enum class Side : std::uint8_t { Buy, Sell };
enum class OrderType : std::uint8_t { Limit, Market };
enum class TimeInForce : std::uint8_t { GTC, IOC, FOK }; // Good 'til Cancelled, Immediate or Cancel, Fill or Kill
using TraderId = std::uint32_t;
using SymbolId = std::uint32_t;

struct Order {
    OrderId id;
    Price price;
    Timestamp timestamp;
    Quantity originalQuantity;
    Quantity remainingQuantity;
    Quantity cumulativeFilledQuantity;
    TraderId traderId;
    SymbolId symbolId;
    Side side;
    OrderType orderType;
    TimeInForce timeInForce;

    void fill(Quantity q) noexcept {
        assert(q <= remainingQuantity);               // can't trade more than is left
        remainingQuantity        -= q;
        cumulativeFilledQuantity += q;
        assert(originalQuantity == remainingQuantity + cumulativeFilledQuantity);
    }

    bool isFilled() const noexcept { return remainingQuantity == 0; }
};




} // namespace lob