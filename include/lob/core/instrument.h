#pragma once

#include "lob/common/types.h"

namespace lob {
    struct InstrumentConfig {
        Price tickMultiple = 1; //every integer is allowed
        Price refPrice = 0; // center of the band (static by default, changes by last trade price later)
        Price bandTicks = 0; //max distance form refPrice = 0, 0 means the band is disabled
        std::uint8_t allowedTIF = 0b111; //bitmask, follows enum order
    };
} //namespace lob