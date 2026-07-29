#include <catch2/catch_test_macros.hpp>

#include "uavmv/telemetry/telemetry_health_monitor.hpp"

namespace {
using namespace std::chrono_literals;
using uavmv::telemetry::HealthState;

uavmv::telemetry::TelemetrySnapshot fresh(uavmv::time::TimePoint now) {
  const uavmv::telemetry::TimedValidity valid{now, true};
  return {valid, valid, valid, valid, valid, valid, 100.0};
}

struct Fixture {
  uavmv::time::ManualClock clock;
  uavmv::logging::EventLogger logger;
  uavmv::telemetry::TelemetryRules rules;
  uavmv::telemetry::TelemetryHealthMonitor monitor{clock, rules, logger};
};
}  // namespace

TEST_CASE("fresh valid telemetry is healthy") {
  Fixture f;
  REQUIRE(f.monitor.evaluate(fresh(f.clock.now())).overall == HealthState::Healthy);
}

TEST_CASE("heartbeat crosses documented freshness boundaries") {
  Fixture f;
  auto snapshot = fresh(f.clock.now());
  f.clock.advance(501ms);
  snapshot.gps.updated_at = f.clock.now();
  snapshot.position.updated_at = f.clock.now();
  snapshot.attitude.updated_at = f.clock.now();
  snapshot.command_ack.updated_at = f.clock.now();
  snapshot.mission_sync.updated_at = f.clock.now();
  REQUIRE(f.monitor.evaluate(snapshot).overall == HealthState::Degraded);
}

TEST_CASE("stale heartbeat becomes critical") {
  Fixture f;
  auto snapshot = fresh(f.clock.now());
  f.clock.advance(1500ms);
  snapshot.gps.updated_at = f.clock.now();
  snapshot.position.updated_at = f.clock.now();
  snapshot.attitude.updated_at = f.clock.now();
  snapshot.command_ack.updated_at = f.clock.now();
  snapshot.mission_sync.updated_at = f.clock.now();
  REQUIRE(f.monitor.evaluate(snapshot).overall == HealthState::Critical);
}

TEST_CASE("invalid GPS is classified as lost") {
  Fixture f;
  auto snapshot = fresh(f.clock.now());
  snapshot.gps.valid = false;
  const auto assessment = f.monitor.evaluate(snapshot);
  REQUIRE(assessment.overall == HealthState::Lost);
  REQUIRE(assessment.signals[1].state == HealthState::Lost);
}

TEST_CASE("battery thresholds are deterministic") {
  Fixture f;
  auto snapshot = fresh(f.clock.now());
  snapshot.battery_percent = 15.0;
  REQUIRE(f.monitor.evaluate(snapshot).overall == HealthState::Critical);
}

TEST_CASE("worst simultaneous signal determines aggregate health") {
  Fixture f;
  auto snapshot = fresh(f.clock.now());
  snapshot.battery_percent = 25.0;
  snapshot.position.valid = false;
  REQUIRE(f.monitor.evaluate(snapshot).overall == HealthState::Lost);
}

TEST_CASE("health deterioration is immediate") {
  Fixture f;
  REQUIRE(f.monitor.evaluate(fresh(f.clock.now())).overall == HealthState::Healthy);
  auto invalid = fresh(f.clock.now());
  invalid.heartbeat.valid = false;
  REQUIRE(f.monitor.evaluate(invalid).overall == HealthState::Lost);
}

TEST_CASE("healthy recovery requires configured consecutive samples") {
  Fixture f;
  auto invalid = fresh(f.clock.now());
  invalid.heartbeat.valid = false;
  REQUIRE(f.monitor.evaluate(invalid).overall == HealthState::Lost);
  REQUIRE(f.monitor.evaluate(fresh(f.clock.now())).overall == HealthState::Lost);
  REQUIRE(f.monitor.evaluate(fresh(f.clock.now())).overall == HealthState::Healthy);
}

TEST_CASE("health assessment is logged with manual clock timestamp") {
  Fixture f;
  f.clock.advance(37ms);
  f.monitor.evaluate(fresh(f.clock.now()));
  REQUIRE(f.logger.records().size() == 1);
  REQUIRE(f.logger.records().front().kind == uavmv::logging::RecordKind::HealthAssessment);
  REQUIRE(f.logger.records().front().timestamp.time_since_epoch() == 37ms);
}
