// Inbound messages: the requests a trader can send to the engine.
#pragma  once

#include "../common/types.h"
#include <variant>

namespace lob {
    // Place a new order.
    // Fields go largest to smallest to minimise padding bytes (32 bytes total).
    struct NewOrder {
        OrderId client_order_id; // ID chosen by the trader to track their own order
        Price price;             // in ticks; ignored for Market orders
        Quantity qty;
        SymbolId symbolId;
        TraderId traderId;
        Side side;
        OrderType type;
        TimeInForce tif;
    };

    // Remove a resting order.
    struct CancelOrder {
        OrderId orderId;
    };

    // Change a resting order's price and/or quantity.
    struct ModifyOrder {
        OrderId orderId;
        Price newPrice;
        Quantity newQty;
    };

    // A variant holds exactly one of these types at a time (a type-safe union).
    // Check which with std::holds_alternative, read it with std::get or std::visit.
    using Command = std::variant<NewOrder, CancelOrder, ModifyOrder>;

} //namespace lob