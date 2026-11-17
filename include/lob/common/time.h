// Two clocks, both monotonic (never go backwards):
//   now()          - nanoseconds, for timestamping orders
//   cycles::read() - raw CPU counter, for benchmarking; convert with cycles::to_ns()
#pragma once

#include <cstdint>
#include <chrono>

// Intel/AMD only: needed for __rdtsc()
#if defined(__x86_64__) || defined(_M_X64)
#include <x86intrin.h>   // __rdtsc
#endif

namespace lob {

// nanoseconds since an epoch, monotonic (never goes backwards)
// Only differences between timestamps are meaningful.
using Timestamp = std::uint64_t;

// `inline` lets a function be defined in a header that many files include.
inline Timestamp now() noexcept {
    using namespace std::chrono;
    return static_cast<Timestamp>(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count());
}

namespace cycles {

    //raw hardware tick counter
    //units are not ns, so must convert using to_ns()
    inline std::uint64_t read() noexcept {
    #if defined(__x86_64__) || defined(_M_X64)
        return __rdtsc(); // Intel/AMD time-stamp counter
    #elif defined(__aarch64__)
        // ARM (e.g. Apple Silicon): isb stops the CPU reading the counter early,
        // mrs copies the counter register into `value`.
        std::uint64_t value;
        asm volatile("isb\n\tmrs %0, cntvct_el0" : "=r"(value) : : "memory");
        return value;
    #else
        return now(); //fallback to ns
    #endif
    }


// How many ns one counter tick lasts.
inline double calibrate_ns_per_tick() noexcept {
    #if defined(__aarch64__)
        // On ARM, the counter frequency is available in cntfrq_el0
        std::uint64_t freq;
        asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
        return 1e9 / static_cast<double>(freq);
    #elif defined(__x86_64__) || defined(_M_X64)
       // x86 doesn't report it, so measure: spin 20 ms and compare both clocks.
       using namespace std::chrono;
       const auto t0 = steady_clock::now();
       const auto c0 = read();
       while (steady_clock::now() - t0 < milliseconds(20)) {}
       const auto c1 = read();
       const auto t1 = steady_clock::now();
       return duration<double, std::nano>(t1 - t0).count() / static_cast<double>(c1 - c0);
    #else
        return 1.0; //fallback to 1 ns per tick
    #endif
}

//calibrate once on first use
// (a static local is initialised only on the first call)
inline double ns_per_tick() noexcept {
    static const double ns_per_tick_value = calibrate_ns_per_tick();
    return ns_per_tick_value;
}

// Usage: to_ns(read() - start)
inline double to_ns(std::uint64_t ticks) noexcept {
    return static_cast<double>(ticks) * ns_per_tick();
}

} // namespace cycles
} // namespace lob
