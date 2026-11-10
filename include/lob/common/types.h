#pragma once
#include <cstdint>

namespace lob {
    using OrderId = std::uint64_t;
    using Price = std::int64_t;
    using Quantity = std::uint32_t;

    enum class Side : std::uint8_t {Buy, Sell};

    struct Order {
        OrderId id;
        Price price;
        Quantity quantity;
        Side side;
        uint64_t timestamp;
    };
    
} //namespace lob