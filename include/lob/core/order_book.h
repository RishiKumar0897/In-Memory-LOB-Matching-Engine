// The order book. Placeholder for now; real methods come in later phases.
#pragma once
#include "lob/common/types.h"

#include <cstddef>

namespace lob {
class OrderBook {
public:
    std::size_t order_count() const; // number of resting orders
};
} // namespace lob
