#include <catch2/catch_test_macros.hpp>

#include <filesystem>

#include "uavmv/failsafe/failsafe_supervisor.hpp"
#include "uavmv/faults/fault_injector.hpp"
#include "uavmv/faults/fault_scheduler.hpp"
#include "uavmv/simulation/scenario_loader.hpp"
#include "uavmv/telemetry/telemetry_health_monitor.hpp"

namespace {
uavmv::telemetry::TelemetrySnapshot fresh(uavmv::time::TimePoint now) {
  const uavmv::telemetry::TimedValidity valid{now, true};
  return {valid, valid, valid, valid, valid, valid, valid, 100.0};
}

std::filesystem::path configs() {
  return std::filesystem::path{UAVMV_SOURCE_DIR} / "experiments" / "configs";
}
}  // namespace

TEST_CASE("health-affecting scenario oracles match composed Phase 2 policy") {
  for (const auto& entry : std::filesystem::directory_iterator(configs())) {
    if (entry.path().extension() != ".json") continue;
    const auto scenario = uavmv::simulation::ScenarioLoader::load_json(entry.path());
    uavmv::faults::FaultScheduler scheduler{scenario.faults};
    uavmv::time::ManualClock clock{
        uavmv::time::TimePoint{scenario.faults.front().start}};
    const auto active = scheduler.active_at(clock.now());
    const auto effects = uavmv::faults::FaultInjector{}.apply(fresh(clock.now()), active, 0);

    if (effects.reject_next_command) {
      REQUIRE(scenario.expected.action == uavmv::failsafe::FailsafeAction::None);
      continue;
    }

    uavmv::logging::EventLogger logger;
    uavmv::telemetry::TelemetryHealthMonitor monitor{
        clock, uavmv::telemetry::TelemetryRules{}, logger};
    uavmv::failsafe::FailsafeSupervisor supervisor{
        clock, uavmv::failsafe::FailsafePolicy{}, logger};
    const auto decision = supervisor.evaluate(monitor.evaluate(effects.telemetry));
    INFO("scenario " << scenario.id);
    REQUIRE(decision.action == scenario.expected.action);
  }
}

TEST_CASE("transient scenario clears at the documented end boundary") {
  const auto scenario = uavmv::simulation::ScenarioLoader::load_json(
      configs() / "F12-transient-recovery.json");
  const uavmv::faults::FaultScheduler scheduler{scenario.faults};
  const auto& fault = scenario.faults.front();
  REQUIRE(scheduler.active_at(uavmv::time::TimePoint{fault.start}).size() == 1);
  REQUIRE(scheduler.active_at(uavmv::time::TimePoint{fault.start + fault.duration}).empty());
  REQUIRE(scenario.expected.recovery);
}
