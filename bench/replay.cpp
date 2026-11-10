#include "order_book.h"
#include <iostream>

int main() {
    lob::OrderBook book;
    std::cout << "orders: " << book.order_count() << '\n';
}