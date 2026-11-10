#pragma once

#include <cstdint>
#include "time.h"

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
    Quantity originalQuantity;
    Quantity remainingQuantity;
    Side side;
    Timestamp timestamp;
    OrderType orderType;
    TimeInForce timeInForce;
    TraderId traderId;
    SymbolId symbolId;
};


} // namespace lob