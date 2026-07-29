#include <catch2/catch_test_macros.hpp>

#include "uavmv/logging/event_logger.hpp"
#include "uavmv/mission/mission_state_machine.hpp"
#include "uavmv/time/clock.hpp"

namespace {
struct Fixture {
  uavmv::time::ManualClock clock;
  uavmv::logging::EventLogger logger;
  uavmv::mission::MissionContext context;
  uavmv::mission::MissionStateMachine machine{context, clock, logger};
};
}  // namespace

TEST_CASE("mission starts in boot") {
  Fixture f;
  REQUIRE(f.machine.state() == uavmv::mission::MissionState::Boot);
}

TEST_CASE("system ready moves boot to preflight") {
  Fixture f;
  const auto result = f.machine.handle(uavmv::mission::MissionEvent::SystemReady);
  REQUIRE(result.accepted);
  REQUIRE(result.next_state == uavmv::mission::MissionState::Preflight);
}

TEST_CASE("preflight event records readiness without leaving preflight") {
  Fixture f;
  f.machine.handle(uavmv::mission::MissionEvent::SystemReady);
  const auto result = f.machine.handle(uavmv::mission::MissionEvent::PreflightOk);
  REQUIRE(result.accepted);
  REQUIRE(f.context.preflight_ready);
  REQUIRE(f.machine.state() == uavmv::mission::MissionState::Preflight);
}

TEST_CASE("arm confirmation requires preflight readiness") {
  Fixture f;
  f.machine.handle(uavmv::mission::MissionEvent::SystemReady);
  const auto result = f.machine.handle(uavmv::mission::MissionEvent::ArmConfirmed);
  REQUIRE_FALSE(result.accepted);
  REQUIRE(result.reason == uavmv::mission::ReasonCode::PreflightNotReady);
  REQUIRE(f.machine.state() == uavmv::mission::MissionState::Preflight);
}

TEST_CASE("valid takeoff path reaches mission") {
  Fixture f;
  f.machine.handle(uavmv::mission::MissionEvent::SystemReady);
  f.machine.handle(uavmv::mission::MissionEvent::PreflightOk);
  f.machine.handle(uavmv::mission::MissionEvent::ArmConfirmed);
  f.machine.handle(uavmv::mission::MissionEvent::TakeoffConfirmed);
  const auto result = f.machine.handle(uavmv::mission::MissionEvent::WaypointReached);
  REQUIRE(result.accepted);
  REQUIRE(f.machine.state() == uavmv::mission::MissionState::Mission);
}

TEST_CASE("invalid event leaves state unchanged") {
  Fixture f;
  const auto result = f.machine.handle(uavmv::mission::MissionEvent::LandRequested);
  REQUIRE_FALSE(result.accepted);
  REQUIRE(result.reason == uavmv::mission::ReasonCode::InvalidStateTransition);
  REQUIRE(f.machine.state() == uavmv::mission::MissionState::Boot);
}

TEST_CASE("operator abort is accepted from active state") {
  Fixture f;
  f.machine.handle(uavmv::mission::MissionEvent::SystemReady);
  const auto result = f.machine.handle(uavmv::mission::MissionEvent::OperatorAbort);
  REQUIRE(result.accepted);
  REQUIRE(result.reason == uavmv::mission::ReasonCode::OperatorAbort);
  REQUIRE(f.context.operator_abort);
  REQUIRE(f.machine.state() == uavmv::mission::MissionState::Aborted);
}

TEST_CASE("operator abort cannot transition a completed mission") {
  Fixture f;
  f.context.state = uavmv::mission::MissionState::Completed;
  const auto result = f.machine.handle(uavmv::mission::MissionEvent::OperatorAbort);
  REQUIRE_FALSE(result.accepted);
  REQUIRE(result.reason == uavmv::mission::ReasonCode::TerminalMissionState);
}

TEST_CASE("completed mission rejects further normal events") {
  Fixture f;
  f.context.state = uavmv::mission::MissionState::Completed;
  const auto result = f.machine.handle(uavmv::mission::MissionEvent::SystemReady);
  REQUIRE_FALSE(result.accepted);
  REQUIRE(f.machine.state() == uavmv::mission::MissionState::Completed);
}

TEST_CASE("transition records include deterministic timestamp and result") {
  Fixture f;
  f.clock.advance(std::chrono::milliseconds{42});
  f.machine.handle(uavmv::mission::MissionEvent::SystemReady);
  REQUIRE(f.logger.records().size() == 1);
  const auto& record = f.logger.records().front();
  REQUIRE(record.timestamp.time_since_epoch() == std::chrono::milliseconds{42});
  REQUIRE(record.accepted);
  REQUIRE(record.previous_state == uavmv::mission::MissionState::Boot);
  REQUIRE(record.next_state == uavmv::mission::MissionState::Preflight);
}

TEST_CASE("mission completion uses return and landing states") {
  Fixture f;
  f.context = {.state = uavmv::mission::MissionState::Mission, .armed = true};
  REQUIRE(f.machine.handle(uavmv::mission::MissionEvent::MissionComplete).accepted);
  REQUIRE(f.machine.handle(uavmv::mission::MissionEvent::LandRequested).accepted);
  REQUIRE(f.machine.handle(uavmv::mission::MissionEvent::MissionComplete).accepted);
  REQUIRE(f.machine.state() == uavmv::mission::MissionState::Completed);
  REQUIRE_FALSE(f.context.armed);
}
