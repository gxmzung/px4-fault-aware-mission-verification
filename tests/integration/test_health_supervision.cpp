#include <catch2/catch_test_macros.hpp>

#include "uavmv/failsafe/failsafe_supervisor.hpp"
#include "uavmv/mission/mission_state_machine.hpp"
#include "uavmv/telemetry/telemetry_health_monitor.hpp"

namespace {
uavmv::telemetry::TelemetrySnapshot fresh(uavmv::time::TimePoint now) {
  const uavmv::telemetry::TimedValidity valid{now, true};
  return {valid, valid, valid, valid, valid, valid, valid, 100.0};
}

struct System {
  uavmv::time::ManualClock clock;
  uavmv::logging::EventLogger logger;
  uavmv::mission::MissionContext context{.state = uavmv::mission::MissionState::Mission,
                                         .armed = true,
                                         .mission_loaded = true};
  uavmv::mission::MissionStateMachine mission{context, clock, logger};
  uavmv::telemetry::TelemetryHealthMonitor monitor{
      clock, uavmv::telemetry::TelemetryRules{}, logger};
  uavmv::failsafe::FailsafeSupervisor supervisor{
      clock, uavmv::failsafe::FailsafePolicy{}, logger};
};
}  // namespace

TEST_CASE("degraded health can be explicitly coordinated into hold") {
  System s;
  auto snapshot = fresh(s.clock.now());
  snapshot.battery_percent = 25.0;
  const auto decision = s.supervisor.evaluate(s.monitor.evaluate(snapshot));
  REQUIRE(decision.action == uavmv::failsafe::FailsafeAction::Hold);
  REQUIRE(s.mission.handle(uavmv::mission::MissionEvent::HoldRequested).accepted);
  REQUIRE(s.mission.state() == uavmv::mission::MissionState::Hold);
}

TEST_CASE("lost health can be explicitly coordinated into land") {
  System s;
  auto snapshot = fresh(s.clock.now());
  snapshot.heartbeat.valid = false;
  const auto decision = s.supervisor.evaluate(s.monitor.evaluate(snapshot));
  REQUIRE(decision.action == uavmv::failsafe::FailsafeAction::Land);
  REQUIRE(s.mission.handle(uavmv::mission::MissionEvent::LandRequested).accepted);
  REQUIRE(s.mission.state() == uavmv::mission::MissionState::Land);
}

TEST_CASE("health evaluation alone never mutates mission state") {
  System s;
  auto snapshot = fresh(s.clock.now());
  snapshot.gps.valid = false;
  s.supervisor.evaluate(s.monitor.evaluate(snapshot));
  REQUIRE(s.mission.state() == uavmv::mission::MissionState::Mission);
}
