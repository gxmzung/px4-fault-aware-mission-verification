#include <catch2/catch_test_macros.hpp>

#include "uavmv/command/command_guard.hpp"

namespace {
using uavmv::command::Command;
using uavmv::command::CommandType;
using uavmv::mission::MissionState;
using uavmv::mission::ReasonCode;

struct Fixture {
  uavmv::time::ManualClock clock;
  uavmv::logging::EventLogger logger;
  uavmv::command::CommandGuard guard{clock, logger};
  uavmv::mission::MissionContext context;

  Command command(CommandType type, std::uint64_t sequence = 1) {
    return {type, sequence, clock.now()};
  }
};
}  // namespace

TEST_CASE("takeoff is rejected in boot") {
  Fixture f;
  const auto result = f.guard.evaluate(f.command(CommandType::Takeoff), f.context);
  REQUIRE_FALSE(result.allowed);
  REQUIRE(result.reason == ReasonCode::InvalidStateTransition);
}

TEST_CASE("arm requires preflight readiness") {
  Fixture f;
  f.context.state = MissionState::Preflight;
  const auto result = f.guard.evaluate(f.command(CommandType::Arm), f.context);
  REQUIRE_FALSE(result.allowed);
  REQUIRE(result.reason == ReasonCode::PreflightNotReady);
}

TEST_CASE("arm is allowed after preflight readiness") {
  Fixture f;
  f.context.state = MissionState::Preflight;
  f.context.preflight_ready = true;
  REQUIRE(f.guard.evaluate(f.command(CommandType::Arm), f.context).allowed);
}

TEST_CASE("takeoff requires armed vehicle") {
  Fixture f;
  f.context.state = MissionState::Armed;
  const auto result = f.guard.evaluate(f.command(CommandType::Takeoff), f.context);
  REQUIRE_FALSE(result.allowed);
  REQUIRE(result.reason == ReasonCode::VehicleNotArmed);
}

TEST_CASE("takeoff is allowed in armed state with armed vehicle") {
  Fixture f;
  f.context.state = MissionState::Armed;
  f.context.armed = true;
  REQUIRE(f.guard.evaluate(f.command(CommandType::Takeoff), f.context).allowed);
}

TEST_CASE("mission start requires a loaded mission") {
  Fixture f;
  f.context.state = MissionState::Takeoff;
  const auto result = f.guard.evaluate(f.command(CommandType::StartMission), f.context);
  REQUIRE_FALSE(result.allowed);
  REQUIRE(result.reason == ReasonCode::MissionNotLoaded);
}

TEST_CASE("return to home is allowed during mission") {
  Fixture f;
  f.context.state = MissionState::Mission;
  REQUIRE(f.guard.evaluate(f.command(CommandType::ReturnToHome), f.context).allowed);
}

TEST_CASE("completed mission rejects command") {
  Fixture f;
  f.context.state = MissionState::Completed;
  const auto result = f.guard.evaluate(f.command(CommandType::StartMission), f.context);
  REQUIRE_FALSE(result.allowed);
  REQUIRE(result.reason == ReasonCode::TerminalMissionState);
}

TEST_CASE("duplicate command sequence is rejected") {
  Fixture f;
  f.context.state = MissionState::Mission;
  f.context.last_command_sequence = 10;
  const auto result = f.guard.evaluate(f.command(CommandType::Hold, 10), f.context);
  REQUIRE_FALSE(result.allowed);
  REQUIRE(result.reason == ReasonCode::DuplicateCommand);
}

TEST_CASE("guard records each command decision") {
  Fixture f;
  f.guard.evaluate(f.command(CommandType::Takeoff), f.context);
  REQUIRE(f.logger.records().size() == 1);
  REQUIRE(f.logger.records().front().kind == uavmv::logging::RecordKind::CommandDecision);
  REQUIRE_FALSE(f.logger.records().front().accepted);
}
