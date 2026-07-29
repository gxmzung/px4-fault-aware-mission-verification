#include <catch2/catch_test_macros.hpp>

#include "uavmv/vehicle/virtual_fc.hpp"

namespace {
uavmv::command::Command command(uavmv::command::CommandType type,
                                std::uint64_t sequence = 1) {
  return {type, sequence, uavmv::time::TimePoint{}};
}
}  // namespace

TEST_CASE("virtual FC acknowledges successful arm command") {
  uavmv::vehicle::VirtualFC vehicle;
  const auto ack = vehicle.execute(command(uavmv::command::CommandType::Arm));
  REQUIRE(ack.accepted);
  REQUIRE(vehicle.state().armed);
  REQUIRE(vehicle.state().mode == uavmv::vehicle::VehicleMode::Armed);
}

TEST_CASE("virtual FC can reject the next command") {
  uavmv::vehicle::VirtualFC vehicle;
  vehicle.set_next_command_result(false);
  const auto ack = vehicle.execute(command(uavmv::command::CommandType::Arm));
  REQUIRE_FALSE(ack.accepted);
  REQUIRE(ack.reason == uavmv::mission::ReasonCode::CommandRejected);
  REQUIRE_FALSE(vehicle.state().armed);
}

TEST_CASE("configured result applies to one command only") {
  uavmv::vehicle::VirtualFC vehicle;
  vehicle.set_next_command_result(false);
  REQUIRE_FALSE(vehicle.execute(command(uavmv::command::CommandType::Arm, 1)).accepted);
  REQUIRE(vehicle.execute(command(uavmv::command::CommandType::Arm, 2)).accepted);
}

TEST_CASE("virtual FC exposes minimal mode changes") {
  uavmv::vehicle::VirtualFC vehicle;
  vehicle.execute(command(uavmv::command::CommandType::Arm, 1));
  vehicle.execute(command(uavmv::command::CommandType::Takeoff, 2));
  REQUIRE(vehicle.state().mode == uavmv::vehicle::VehicleMode::TakingOff);
}
