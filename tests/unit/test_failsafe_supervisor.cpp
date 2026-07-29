#include <catch2/catch_test_macros.hpp>

#include "uavmv/failsafe/failsafe_supervisor.hpp"

namespace {
using uavmv::failsafe::FailsafeAction;
using uavmv::telemetry::HealthState;

uavmv::telemetry::HealthAssessment assessment(HealthState state) {
  std::array<uavmv::telemetry::SignalHealth, 8> signals{};
  return {signals, state};
}

struct Fixture {
  uavmv::time::ManualClock clock;
  uavmv::logging::EventLogger logger;
  uavmv::failsafe::FailsafePolicy policy;
  uavmv::failsafe::FailsafeSupervisor supervisor{clock, policy, logger};
};
}  // namespace

TEST_CASE("healthy telemetry requires no failsafe action") {
  Fixture f;
  REQUIRE(f.supervisor.evaluate(assessment(HealthState::Healthy)).action ==
          FailsafeAction::None);
}

TEST_CASE("degraded telemetry selects hold by default") {
  Fixture f;
  REQUIRE(f.supervisor.evaluate(assessment(HealthState::Degraded)).action ==
          FailsafeAction::Hold);
}

TEST_CASE("critical telemetry selects return to home by default") {
  Fixture f;
  REQUIRE(f.supervisor.evaluate(assessment(HealthState::Critical)).action ==
          FailsafeAction::ReturnToHome);
}

TEST_CASE("lost telemetry selects land by default") {
  Fixture f;
  REQUIRE(f.supervisor.evaluate(assessment(HealthState::Lost)).action ==
          FailsafeAction::Land);
}

TEST_CASE("policy actions are configurable") {
  uavmv::time::ManualClock clock;
  uavmv::logging::EventLogger logger;
  uavmv::failsafe::FailsafePolicy policy;
  policy.lost_action = FailsafeAction::ReturnToHome;
  uavmv::failsafe::FailsafeSupervisor supervisor{clock, policy, logger};
  REQUIRE(supervisor.evaluate(assessment(HealthState::Lost)).action ==
          FailsafeAction::ReturnToHome);
}

TEST_CASE("recovery holds active action until enough healthy samples") {
  Fixture f;
  f.supervisor.evaluate(assessment(HealthState::Degraded));
  const auto first = f.supervisor.evaluate(assessment(HealthState::Healthy));
  REQUIRE(first.action == FailsafeAction::Hold);
  REQUIRE(first.recovery);
  const auto second = f.supervisor.evaluate(assessment(HealthState::Healthy));
  REQUIRE(second.action == FailsafeAction::ResumeMission);
  REQUIRE(f.supervisor.active_action() == FailsafeAction::None);
}

TEST_CASE("new degradation resets recovery streak") {
  Fixture f;
  f.supervisor.evaluate(assessment(HealthState::Degraded));
  f.supervisor.evaluate(assessment(HealthState::Healthy));
  f.supervisor.evaluate(assessment(HealthState::Critical));
  REQUIRE(f.supervisor.evaluate(assessment(HealthState::Healthy)).action ==
          FailsafeAction::ReturnToHome);
}

TEST_CASE("supervisor decisions create structured records") {
  Fixture f;
  f.supervisor.evaluate(assessment(HealthState::Lost));
  REQUIRE(f.logger.records().size() == 1);
  REQUIRE(f.logger.records().front().kind == uavmv::logging::RecordKind::FailsafeDecision);
  REQUIRE(f.logger.records().front().subject == "LAND");
}
