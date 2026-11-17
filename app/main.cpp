// Smoke test: proves the app links against the lob library. Prints "orders: 0".
#include "lob/core/order_book.h"

#include <iostream>

int main() {
    lob::OrderBook book;
    std::cout << "orders: " << book.order_count() << '\n';
}
