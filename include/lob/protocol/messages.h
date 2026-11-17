#pragma  once

#include "../common/types.h"
#include <variant>

namespace lob {
    // Fields go largest to smallest to minimise padding bytes (32 bytes total).
    struct NewOrder {
        OrderId client_order_id;
        Price price;
        Quantity qty;
        SymbolId symbolId;
        TraderId traderId;
        Side side;
        OrderType type;
        TimeInForce tif;
    };

    struct CancelOrder {
        OrderId orderId;
    };

    struct ModifyOrder {
        OrderId orderId;
        Price newPrice;
        Quantity newQty;
    };

    using Command = std::variant<NewOrder, CancelOrder, ModifyOrder>;

} //namespace lob