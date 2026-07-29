#include <catch2/catch_test_macros.hpp>

#include "uavmv/command/command_guard.hpp"
#include "uavmv/mission/mission_state_machine.hpp"
#include "uavmv/vehicle/virtual_fc.hpp"

namespace {
using uavmv::command::Command;
using uavmv::command::CommandType;
using uavmv::mission::MissionEvent;
using uavmv::mission::MissionState;

struct System {
  uavmv::time::ManualClock clock;
  uavmv::logging::EventLogger logger;
  uavmv::mission::MissionContext context;
  uavmv::mission::MissionStateMachine mission{context, clock, logger};
  uavmv::command::CommandGuard guard{clock, logger};
  uavmv::vehicle::VirtualFC vehicle;
  std::uint64_t sequence{0};

  bool issue(CommandType type) {
    Command command{type, ++sequence, clock.now()};
    const auto decision = guard.evaluate(command, context);
    if (!decision.allowed) return false;
    const auto ack = vehicle.execute(command);
    if (ack.accepted) context.last_command_sequence = command.sequence;
    return ack.accepted;
  }
};
}  // namespace

TEST_CASE("guarded arm reaches vehicle only after preflight") {
  System s;
  REQUIRE_FALSE(s.issue(CommandType::Arm));
  REQUIRE_FALSE(s.vehicle.state().armed);
  s.mission.handle(MissionEvent::SystemReady);
  s.mission.handle(MissionEvent::PreflightOk);
  REQUIRE(s.issue(CommandType::Arm));
  REQUIRE(s.vehicle.state().armed);
}

TEST_CASE("vehicle rejection does not advance mission state") {
  System s;
  s.mission.handle(MissionEvent::SystemReady);
  s.mission.handle(MissionEvent::PreflightOk);
  s.vehicle.set_next_command_result(false);
  REQUIRE_FALSE(s.issue(CommandType::Arm));
  REQUIRE(s.mission.state() == MissionState::Preflight);
  REQUIRE_FALSE(s.context.armed);
}

TEST_CASE("simplified nominal mission follows documented state sequence") {
  System s;
  REQUIRE(s.mission.handle(MissionEvent::SystemReady).accepted);
  REQUIRE(s.mission.handle(MissionEvent::PreflightOk).accepted);
  REQUIRE(s.issue(CommandType::Arm));
  REQUIRE(s.mission.handle(MissionEvent::ArmConfirmed).accepted);
  REQUIRE(s.issue(CommandType::Takeoff));
  REQUIRE(s.mission.handle(MissionEvent::TakeoffConfirmed).accepted);
  s.context.mission_loaded = true;
  REQUIRE(s.issue(CommandType::StartMission));
  REQUIRE(s.mission.handle(MissionEvent::WaypointReached).accepted);
  REQUIRE(s.issue(CommandType::ReturnToHome));
  REQUIRE(s.mission.handle(MissionEvent::RthRequested).accepted);
  REQUIRE(s.issue(CommandType::Land));
  REQUIRE(s.mission.handle(MissionEvent::LandRequested).accepted);
  REQUIRE(s.mission.handle(MissionEvent::MissionComplete).accepted);
  REQUIRE(s.mission.state() == MissionState::Completed);
}

TEST_CASE("rejected command does not create a transition record") {
  System s;
  REQUIRE_FALSE(s.issue(CommandType::Takeoff));
  REQUIRE(s.logger.records().size() == 1);
  REQUIRE(s.logger.records().front().kind == uavmv::logging::RecordKind::CommandDecision);
  REQUIRE(s.mission.state() == MissionState::Boot);
}
