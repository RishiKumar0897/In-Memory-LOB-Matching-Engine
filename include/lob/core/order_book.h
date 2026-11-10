#pragma once
#include "lob/common/types.h"
#include <cstddef>

namespace lob {
class OrderBook {
public:
    std::size_t order_count() const;
    
};
} //namespace lob