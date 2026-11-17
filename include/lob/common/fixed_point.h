// Converts price text to integer ticks.
// Example, tick 0.01 ({1, 2}): "123.45" -> 123 * 100 + 45 = 12345 ticks.
#pragma once

#include <cstdint>
#include <cstddef>
#include <string_view>
#include <array>


#include "types.h"

namespace lob {
// Tick size = units / 10^decimals. {1, 2} = 0.01, {5, 2} = 0.05.
struct TickSize {
    std::int64_t units;
    std::uint8_t decimals;
};

//need a constexpr table for powers of 10 up to 10^18
// 10^exp. constexpr = can be computed at compile time. Overflows past 10^18.
constexpr std::int64_t pow10(std::uint8_t exp) {
    std::int64_t result = 1;
    for (std::uint8_t i = 0; i < exp; ++i) {
        result *= 10;
    }
    return result;
}

//parsing in 2 stages
// Stage 1: read integer and decimal digits. Stage 2: scale, check tick, convert.
// Returns 0 on invalid input (indistinguishable from a real 0 price).
constexpr std::int64_t parse_price(const char* str, std::size_t len, const TickSize& tick_size) {
    //check if the first character is a minus sign
    // NOTE: reads str[0] even when len == 0.
    bool negative = false;
    std::size_t i = 0;
    if (str[0] == '-') {
        negative = true;
        i = 1;
    }

    //require at least one integer digit
    if (i >= len || str[i] < '0' || str[i] > '9') {
        return 0; //invalid input
    }

    // Build the number digit by digit: "123" -> 1 -> 12 -> 123
    std::int64_t integer_part = 0;
    while (i < len && str[i] >= '0' && str[i] <= '9') {
            integer_part = integer_part * 10 + (str[i] - '0');
            ++i;
        }
    // NOTE: without a '.', trailing junk isn't rejected ("123abc" -> 12300).

    //check for decimal point
    std::int64_t decimal_part = 0;
    if (i < len && str[i] == '.') {
        ++i;
        std::size_t decimal_digits = 0;
        while (i < len && str[i] >= '0' && str[i] <=
    '9' && decimal_digits < tick_size.decimals) {
                decimal_part = decimal_part * 10 + (str[i] - '0');
                ++i;
                ++decimal_digits;
        }
        decimal_part *= pow10(tick_size.decimals - decimal_digits);

        //extra digits are being dropped.
        // any more digits should be 0
        while (i < len && str[i] >= '0' && str[i] <= '9') {
            if (str[i] != '0') {
                return 0; //invalid input
            }
            ++i;
        }
        //check that i == len
        if (i != len) {
            return 0; //invalid input
        }
    }

    // NOTE: no overflow check.
    std::int64_t intscaled = integer_part * pow10(tick_size.decimals) + decimal_part;
    //check the tick, if scaled is not a multiple of tick_size.units, return 0
    if (intscaled % tick_size.units != 0) {
        return 0; //invalid input
    }

    std::int64_t ticks = intscaled / tick_size.units;
    //now apply the sign
    return negative ? -ticks : ticks;
}

// ---- Declared only, not implemented yet ----

//result type to see why parsing fails
enum class PriceError : std::uint8_t { Empty, BadChar, NotOnTick, Overflow};

// Either a value (ok) or an error. NOTE: `error` is uninitialised by default.
struct ParseResult {
    lob::Price value{};
    PriceError error;
    bool ok{};
};

constexpr ParseResult parse_price_result(std::string_view s, TickSize tick) noexcept;

// Fixed-size buffer so formatting doesn't allocate.
struct PriceString {
    std::array<char, 32> buf{};
    std::size_t len{};
    constexpr std::string_view view() const noexcept {
        return std::string_view(buf.data(), len);
    }
};

// Ticks -> text, e.g. 12345 with {1, 2} -> "123.45".
constexpr PriceString format_price(Price ticks, TickSize tick) noexcept;

} // namespace lob
