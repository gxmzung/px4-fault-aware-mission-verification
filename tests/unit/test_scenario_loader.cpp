#include <catch2/catch_test_macros.hpp>

#include <filesystem>

#include "uavmv/simulation/scenario_loader.hpp"

namespace {
std::filesystem::path config(std::string_view filename) {
  return std::filesystem::path{UAVMV_SOURCE_DIR} / "experiments" / "configs" / filename;
}
}  // namespace

TEST_CASE("scenario loader reads required metadata") {
  const auto scenario =
      uavmv::simulation::ScenarioLoader::load_json(config("F03-telemetry-loss.json"));
  REQUIRE(scenario.id == "F03");
  REQUIRE(scenario.faults.size() == 1);
  REQUIRE(scenario.faults.front().type == uavmv::faults::FaultType::TelemetryLoss);
  REQUIRE(scenario.expected.action == uavmv::failsafe::FailsafeAction::Land);
}

TEST_CASE("all twelve scenario definitions load and validate") {
  std::size_t count = 0;
  const auto directory = std::filesystem::path{UAVMV_SOURCE_DIR} / "experiments" / "configs";
  for (const auto& entry : std::filesystem::directory_iterator(directory)) {
    if (entry.path().extension() == ".json") {
      REQUIRE_NOTHROW(uavmv::simulation::ScenarioLoader::load_json(entry.path()));
      ++count;
    }
  }
  REQUIRE(count == 12);
}

TEST_CASE("scenario validation rejects unsupported schema") {
  uavmv::simulation::Scenario scenario;
  scenario.schema_version = 2;
  REQUIRE_THROWS_AS(uavmv::simulation::ScenarioLoader::validate(scenario),
                    std::invalid_argument);
}

TEST_CASE("scenario validation rejects duplicate fault identifiers") {
  using namespace std::chrono_literals;
  uavmv::simulation::Scenario scenario;
  scenario.id = "T";
  scenario.name = "duplicate";
  scenario.run_duration = 1000ms;
  const uavmv::faults::FaultDefinition definition{
      "same", "test", uavmv::faults::FaultType::GpsDropout,
      uavmv::faults::FaultSeverity::High, 0ms, 100ms};
  scenario.faults = {definition, definition};
  REQUIRE_THROWS_AS(uavmv::simulation::ScenarioLoader::validate(scenario),
                    std::invalid_argument);
}
