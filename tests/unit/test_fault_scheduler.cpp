#include <catch2/catch_test_macros.hpp>

#include "uavmv/faults/fault_scheduler.hpp"

namespace {
using namespace std::chrono_literals;

uavmv::faults::FaultDefinition fault(std::string id, uavmv::time::Duration start,
                                     uavmv::time::Duration duration) {
  return {std::move(id), "test", uavmv::faults::FaultType::TelemetryLoss,
          uavmv::faults::FaultSeverity::High, start, duration};
}
}  // namespace

TEST_CASE("scheduler uses inclusive start and exclusive end") {
  uavmv::faults::FaultScheduler scheduler{{fault("F", 100ms, 200ms)}};
  REQUIRE(scheduler.active_at(uavmv::time::TimePoint{99ms}).empty());
  REQUIRE(scheduler.active_at(uavmv::time::TimePoint{100ms}).size() == 1);
  REQUIRE(scheduler.active_at(uavmv::time::TimePoint{299ms}).size() == 1);
  REQUIRE(scheduler.active_at(uavmv::time::TimePoint{300ms}).empty());
}

TEST_CASE("scheduler returns simultaneous active faults") {
  uavmv::faults::FaultScheduler scheduler{
      {fault("A", 100ms, 300ms), fault("B", 200ms, 300ms)}};
  REQUIRE(scheduler.active_at(uavmv::time::TimePoint{250ms}).size() == 2);
}

TEST_CASE("scheduler sorts definitions by activation time") {
  uavmv::faults::FaultScheduler scheduler{
      {fault("late", 500ms, 100ms), fault("early", 100ms, 100ms)}};
  REQUIRE(scheduler.definitions().front().id == "early");
}

TEST_CASE("scheduler rejects nonpositive fault duration") {
  REQUIRE_THROWS_AS(uavmv::faults::FaultScheduler{{fault("F", 0ms, 0ms)}},
                    std::invalid_argument);
}
