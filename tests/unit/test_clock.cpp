#include <catch2/catch_test_macros.hpp>

#include <chrono>

#include "uavmv/time/clock.hpp"

using namespace std::chrono_literals;

TEST_CASE("manual clock starts at the configured time") {
  uavmv::time::ManualClock clock{uavmv::time::TimePoint{250ms}};
  REQUIRE(clock.now().time_since_epoch() == 250ms);
}

TEST_CASE("manual clock advances deterministically") {
  uavmv::time::ManualClock clock;
  clock.advance(100ms);
  clock.advance(25ms);
  REQUIRE(clock.now().time_since_epoch() == 125ms);
}

TEST_CASE("manual clock can be set without sleeping") {
  uavmv::time::ManualClock clock;
  clock.set(uavmv::time::TimePoint{900ms});
  REQUIRE(clock.now().time_since_epoch() == 900ms);
}
