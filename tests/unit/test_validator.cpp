#include "lob/core/validator.h"
#include <gtest/gtest.h>
#include <limits>

namespace {

lob::NewOrder good_limit() {
    return {.client_order_id = 1, .price = 10000, .qty = 100,
            .symbolId = 1, .traderId = 1, .side = lob::Side::Buy,
            .type = lob::OrderType::Limit, .tif = lob::TimeInForce::GTC};
}

lob::NewOrder good_market() {
    return {.client_order_id = 2, .price = 0, .qty = 100,
            .symbolId = 1, .traderId = 1, .side = lob::Side::Buy,
            .type = lob::OrderType::Market, .tif = lob::TimeInForce::IOC};
}

lob::InstrumentConfig default_config() {
    return {};
}

// bitmask helper matching validator's (1u << tif) convention
constexpr std::uint8_t tif_bit(lob::TimeInForce t) {
    return static_cast<std::uint8_t>(1u << static_cast<unsigned>(t));
}

} //namespace

// ---------- happy paths ----------

TEST(Validator, AcceptsGoodLimitOrder) {
    EXPECT_EQ(lob::validate(good_limit(), default_config()) , lob::RejectReason::None);
}

TEST(Validator, AcceptsGoodMarketOrder) {
    EXPECT_EQ(lob::validate(good_market(), default_config()), lob::RejectReason::None);
}

TEST(Validator, AcceptsSellSide) {
    auto o = good_limit();
    o.side = lob::Side::Sell;
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::None);
}

TEST(Validator, DefaultConfigAllowsAllTIFsForLimit) {
    for (auto tif : {lob::TimeInForce::GTC, lob::TimeInForce::IOC, lob::TimeInForce::FOK}) {
        auto o = good_limit();
        o.tif = tif;
        EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::None)
            << "tif=" << static_cast<int>(tif);
    }
}

// validate is constexpr: make sure it really is usable at compile time
static_assert(lob::validate(lob::NewOrder{.client_order_id = 1, .price = 5, .qty = 1,
                                          .symbolId = 1, .traderId = 1, .side = lob::Side::Buy,
                                          .type = lob::OrderType::Limit, .tif = lob::TimeInForce::GTC},
                            lob::InstrumentConfig{}) == lob::RejectReason::None);

// ---------- quantity ----------

TEST(Validator, RejectsZeroQty) {
    auto o = good_limit();
    o.qty = 0;
    EXPECT_EQ(lob::validate(o, default_config()) , lob::RejectReason::InvalidQuantity);
}

TEST(Validator, RejectsZeroQtyMarket) {
    auto o = good_market();
    o.qty = 0;
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::InvalidQuantity);
}

TEST(Validator, AcceptsMinAndMaxQty) {
    auto o = good_limit();
    o.qty = 1;
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::None);
    o.qty = std::numeric_limits<lob::Quantity>::max();
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::None);
}

// ---------- limit price ----------

TEST(Validator, RejectsZeroPriceLimit) {
    auto o = good_limit();
    o.price = 0;
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::InvalidPrice);
}

TEST(Validator, RejectsNegativePriceLimit) {
    auto o = good_limit();
    o.price = -1;
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::InvalidPrice);
}

TEST(Validator, AcceptsPriceOfOneTick) {
    auto o = good_limit();
    o.price = 1;
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::None);
}

// ---------- tick grid ----------

TEST(Validator, AcceptsPriceOnTickGrid) {
    auto cfg = default_config();
    cfg.tickMultiple = 5;
    auto o = good_limit();
    o.price = 10005;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::None);
}

TEST(Validator, RejectsPriceOffTickGrid) {
    auto cfg = default_config();
    cfg.tickMultiple = 5;
    auto o = good_limit();
    o.price = 10003;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::InvalidPrice);
}

TEST(Validator, PriceEqualToTickMultipleIsValid) {
    auto cfg = default_config();
    cfg.tickMultiple = 25;
    auto o = good_limit();
    o.price = 25;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::None);
}

TEST(Validator, PriceBelowTickMultipleIsInvalid) {
    auto cfg = default_config();
    cfg.tickMultiple = 25;
    auto o = good_limit();
    o.price = 24;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::InvalidPrice);
}

TEST(Validator, TickGridDoesNotApplyToMarketOrders) {
    auto cfg = default_config();
    cfg.tickMultiple = 7;
    EXPECT_EQ(lob::validate(good_market(), cfg), lob::RejectReason::None);
}

// ---------- market orders ----------

TEST(Validator, RejectsMarketWithNonZeroPrice) {
    auto o = good_market();
    o.price = 10000;
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::InvalidPrice);
}

TEST(Validator, RejectsMarketWithNegativePrice) {
    auto o = good_market();
    o.price = -5;
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::InvalidPrice);
}

TEST(Validator, RejectsMarketGTC) {
    auto o = good_market();
    o.tif = lob::TimeInForce::GTC;
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::InvalidTimeInForce);
}

TEST(Validator, AcceptsMarketIOCAndFOK) {
    for (auto tif : {lob::TimeInForce::IOC, lob::TimeInForce::FOK}) {
        auto o = good_market();
        o.tif = tif;
        EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::None)
            << "tif=" << static_cast<int>(tif);
    }
}

// ---------- price band ----------

TEST(Validator, BandDisabledByDefaultAllowsFarPrice) {
    auto cfg = default_config();
    cfg.refPrice = 100;            // bandTicks stays 0 -> disabled
    auto o = good_limit();
    o.price = 1'000'000;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::None);
}

TEST(Validator, AcceptsPriceInsideBand) {
    auto cfg = default_config();
    cfg.refPrice = 10000;
    cfg.bandTicks = 50;
    auto o = good_limit();
    o.price = 10020;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::None);
    o.price = 9980;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::None);
}

TEST(Validator, AcceptsPriceExactlyOnBandEdge) {
    auto cfg = default_config();
    cfg.refPrice = 10000;
    cfg.bandTicks = 50;
    auto o = good_limit();
    o.price = 10050;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::None);
    o.price = 9950;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::None);
}

TEST(Validator, RejectsPriceAboveBand) {
    auto cfg = default_config();
    cfg.refPrice = 10000;
    cfg.bandTicks = 50;
    auto o = good_limit();
    o.price = 10051;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::PriceBandViolation);
}

TEST(Validator, RejectsPriceBelowBand) {
    auto cfg = default_config();
    cfg.refPrice = 10000;
    cfg.bandTicks = 50;
    auto o = good_limit();
    o.price = 9949;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::PriceBandViolation);
}

TEST(Validator, BandAppliesToSellSideToo) {
    auto cfg = default_config();
    cfg.refPrice = 10000;
    cfg.bandTicks = 50;
    auto o = good_limit();
    o.side = lob::Side::Sell;
    o.price = 9000;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::PriceBandViolation);
}

// ---------- instrument TIF mask ----------

TEST(Validator, RejectsTIFNotAllowedByInstrument) {
    auto cfg = default_config();
    cfg.allowedTIF = tif_bit(lob::TimeInForce::GTC);   // only GTC
    auto o = good_limit();
    o.tif = lob::TimeInForce::IOC;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::InvalidTimeInForce);
    o.tif = lob::TimeInForce::FOK;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::InvalidTimeInForce);
    o.tif = lob::TimeInForce::GTC;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::None);
}

TEST(Validator, EachTIFBitIsIndependent) {
    for (auto allowed : {lob::TimeInForce::GTC, lob::TimeInForce::IOC, lob::TimeInForce::FOK}) {
        auto cfg = default_config();
        cfg.allowedTIF = tif_bit(allowed);
        for (auto tif : {lob::TimeInForce::GTC, lob::TimeInForce::IOC, lob::TimeInForce::FOK}) {
            auto o = good_limit();
            o.tif = tif;
            const auto expected = (tif == allowed) ? lob::RejectReason::None
                                                   : lob::RejectReason::InvalidTimeInForce;
            EXPECT_EQ(lob::validate(o, cfg), expected)
                << "allowed=" << static_cast<int>(allowed) << " tif=" << static_cast<int>(tif);
        }
    }
}

TEST(Validator, EmptyTIFMaskRejectsEverything) {
    auto cfg = default_config();
    cfg.allowedTIF = 0;
    EXPECT_EQ(lob::validate(good_limit(), cfg), lob::RejectReason::InvalidTimeInForce);
    EXPECT_EQ(lob::validate(good_market(), cfg), lob::RejectReason::InvalidTimeInForce);
}

TEST(Validator, InstrumentMaskAppliesToMarketOrders) {
    auto cfg = default_config();
    cfg.allowedTIF = tif_bit(lob::TimeInForce::GTC) | tif_bit(lob::TimeInForce::FOK); // no IOC
    auto o = good_market();   // IOC
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::InvalidTimeInForce);
    o.tif = lob::TimeInForce::FOK;
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::None);
}

// ---------- check precedence (first failing check wins) ----------

TEST(Validator, QtyCheckedBeforePrice) {
    auto o = good_limit();
    o.qty = 0;
    o.price = -1;
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::InvalidQuantity);
}

TEST(Validator, MarketPriceCheckedBeforeMarketGTC) {
    auto o = good_market();
    o.price = 100;
    o.tif = lob::TimeInForce::GTC;
    EXPECT_EQ(lob::validate(o, default_config()), lob::RejectReason::InvalidPrice);
}

TEST(Validator, TickCheckedBeforeBand) {
    auto cfg = default_config();
    cfg.tickMultiple = 5;
    cfg.refPrice = 10000;
    cfg.bandTicks = 10;
    auto o = good_limit();
    o.price = 20003;   // off-grid AND out of band
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::InvalidPrice);
}

TEST(Validator, BandCheckedBeforeInstrumentTIF) {
    auto cfg = default_config();
    cfg.refPrice = 10000;
    cfg.bandTicks = 10;
    cfg.allowedTIF = 0;
    auto o = good_limit();
    o.price = 20000;   // out of band AND TIF disallowed
    EXPECT_EQ(lob::validate(o, cfg), lob::RejectReason::PriceBandViolation);
}
