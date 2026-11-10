#pragma once
#include <cstdint>

namespace lob {
    using OrderId = std::uint64_t;
    using Price = std::int64_t;
    using Quantity = std::uint32_t;

    enum class Side : std::uint8_t {Buy, Sell};
    
} //namespace lob