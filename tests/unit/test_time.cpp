#include "lob/common/time.h"
#include <gtest/gtest.h>
#include <thread>


//test that now() never decreases and is monotonic
TEST(Time, NowIsMonotonic) {
    lob::Timestamp prev = lob::now();

    for (int i = 0; i < 100000; i++) {
        lob::Timestamp t = lob::now();
        ASSERT_GE(t, prev);
        prev = t;
    }
}

//test that cycles::read() never decreases
TEST(Cycles, ReadIsMonotonic) {
    std::uint64_t prev = lob::cycles::read();

    for (int i = 0; i < 100000; ++i) {
        std::uint64_t c = lob::cycles::read();
        ASSERT_GE(c, prev);
        prev = c;
    }
}

// test that ns_per_tick() returns the same value every time
TEST(Cycles, NsPerTickIsPlausibleAndStable) {
    const double a = lob::cycles::ns_per_tick();
    EXPECT_GT(a, 0.01);
    EXPECT_LT(a, 1000.0);
    EXPECT_EQ(a, lob::cycles::ns_per_tick());
#if defined(__aarch64__) && defined(__APPLE__)
    EXPECT_NEAR(a, 1e9/24e6, 0.01);
#endif
}

// cycle-measured duration is approximately the same as now() measured duration across a sleep
TEST(Cycles, AgreesWithSteadyClock) {
    const lob::Timestamp t0 = lob::now();
    const std::uint64_t c0 = lob::cycles::read();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    const std::uint64_t c1 = lob::cycles::read();
    const lob::Timestamp t1 = lob::now();

    const double wall_ns = static_cast<double>(t1 - t0);
    const double cycle_ns = lob::cycles::to_ns(c1 - c0);

    EXPECT_NEAR(cycle_ns, wall_ns, wall_ns * 0.05); //within 5%
}


