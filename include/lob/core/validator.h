#pragma once

#include"lob/core/instrument.h"
#include "lob/protocol/execution_report.h"
#include "lob/protocol/messages.h"

namespace lob {
    constexpr RejectReason validate(const NewOrder& order, [[maybe_unused]] const InstrumentConfig& config) noexcept {

        if (order.qty == 0) {
            return RejectReason::InvalidQuantity;
        }

        //market orders carry no price, limit orders must be checked for: positive and an integer
        if (order.type == OrderType::Market) {
            if(order.price != 0)
                return RejectReason::InvalidPrice;
            
            //market orders can't be good-til-cancelled
            if (order.tif == TimeInForce::GTC)
                return RejectReason::InvalidTimeInForce;
        } else {
            if (order.price <= 0)
                return RejectReason::InvalidPrice;
            if (order.price % config.tickMultiple != 0)
                return RejectReason::InvalidPrice;
        }

        //sanity band
        if (config.bandTicks != 0) {
            const Price diff = order.price > config.refPrice ? order.price - config.refPrice
                                                             : config.refPrice - order.price;
            if (diff > config.bandTicks)
                return RejectReason::PriceBandViolation;
        }

        //instrument decides which TIFs are allowed
        const auto bit = 1u << static_cast<unsigned>(order.tif);
        if ((config.allowedTIF & bit) == 0)
            return RejectReason::InvalidTimeInForce;

        
    



        return RejectReason::None;
    }
} //namespace lob