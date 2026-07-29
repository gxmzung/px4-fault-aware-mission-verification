#include <catch2/catch_test_macros.hpp>

#include "uavmv/faults/fault_injector.hpp"

namespace {
using uavmv::faults::FaultSeverity;
using uavmv::faults::FaultType;

uavmv::telemetry::TelemetrySnapshot fresh() {
  const uavmv::telemetry::TimedValidity valid{uavmv::time::TimePoint{}, true};
  return {valid, valid, valid, valid, valid, valid, valid, 100.0};
}

uavmv::faults::FaultDefinition fault(FaultType type, double magnitude = 0.0,
                                     std::uint32_t seed = 0) {
  return {"F", "test", type, FaultSeverity::High, uavmv::time::Duration{0},
          uavmv::time::Duration{1000}, magnitude, seed};
}
}  // namespace

TEST_CASE("telemetry loss invalidates vehicle telemetry but not GPS") {
  const auto effects =
      uavmv::faults::FaultInjector{}.apply(fresh(), {fault(FaultType::TelemetryLoss)}, 0);
  REQUIRE_FALSE(effects.telemetry.heartbeat.valid);
  REQUIRE_FALSE(effects.telemetry.position.valid);
  REQUIRE(effects.telemetry.gps.valid);
}

TEST_CASE("GPS dropout affects only GPS validity") {
  const auto effects =
      uavmv::faults::FaultInjector{}.apply(fresh(), {fault(FaultType::GpsDropout)}, 0);
  REQUIRE_FALSE(effects.telemetry.gps.valid);
  REQUIRE(effects.telemetry.heartbeat.valid);
}

TEST_CASE("command rejection is represented separately from telemetry") {
  const auto effects =
      uavmv::faults::FaultInjector{}.apply(fresh(), {fault(FaultType::CommandRejection)}, 0);
  REQUIRE(effects.reject_next_command);
  REQUIRE(effects.telemetry.heartbeat.valid);
}

TEST_CASE("packet loss is deterministic for seed and sample index") {
  const auto definition = fault(FaultType::TelemetryPacketLoss, 50.0, 7);
  const auto first = uavmv::faults::FaultInjector{}.apply(fresh(), {definition}, 10);
  const auto repeated = uavmv::faults::FaultInjector{}.apply(fresh(), {definition}, 10);
  REQUIRE(first.telemetry.heartbeat.valid == repeated.telemetry.heartbeat.valid);
}

TEST_CASE("battery fault applies configured percentage") {
  const auto effects =
      uavmv::faults::FaultInjector{}.apply(fresh(), {fault(FaultType::BatteryCritical, 9.0)}, 0);
  REQUIRE(effects.telemetry.battery_percent == 9.0);
}

TEST_CASE("multiple active faults compose in input order") {
  const auto effects = uavmv::faults::FaultInjector{}.apply(
      fresh(), {fault(FaultType::GpsDropout), fault(FaultType::CommandAckTimeout)}, 0);
  REQUIRE_FALSE(effects.telemetry.gps.valid);
  REQUIRE_FALSE(effects.telemetry.command_ack.valid);
}

TEST_CASE("offboard loss has a dedicated signal") {
  const auto effects = uavmv::faults::FaultInjector{}.apply(
      fresh(), {fault(FaultType::OffboardHeartbeatLoss)}, 0);
  REQUIRE_FALSE(effects.telemetry.offboard_heartbeat.valid);
  REQUIRE(effects.telemetry.heartbeat.valid);
}
