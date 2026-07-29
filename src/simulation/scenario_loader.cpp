#include "uavmv/simulation/scenario_loader.hpp"

#include <fstream>
#include <set>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace uavmv::simulation {
namespace {

failsafe::FailsafeAction action_from_string(std::string_view value) {
  if (value == "NONE") return failsafe::FailsafeAction::None;
  if (value == "HOLD") return failsafe::FailsafeAction::Hold;
  if (value == "RETURN_TO_HOME") return failsafe::FailsafeAction::ReturnToHome;
  if (value == "LAND") return failsafe::FailsafeAction::Land;
  if (value == "RESUME_MISSION") return failsafe::FailsafeAction::ResumeMission;
  throw std::invalid_argument("unknown expected action: " + std::string(value));
}

}  // namespace

Scenario ScenarioLoader::load_json(const std::filesystem::path& path) {
  std::ifstream input(path);
  if (!input) throw std::runtime_error("unable to open scenario: " + path.string());

  nlohmann::json root;
  input >> root;
  Scenario scenario;
  scenario.schema_version = root.at("schema_version").get<int>();
  scenario.id = root.at("id").get<std::string>();
  scenario.name = root.at("name").get<std::string>();
  scenario.run_duration = time::Duration{root.at("run_duration_ms").get<time::Duration::rep>()};

  for (const auto& item : root.at("faults")) {
    scenario.faults.push_back({
        item.at("id").get<std::string>(),
        item.at("name").get<std::string>(),
        faults::fault_type_from_string(item.at("type").get<std::string>()),
        faults::fault_severity_from_string(item.at("severity").get<std::string>()),
        time::Duration{item.at("start_ms").get<time::Duration::rep>()},
        time::Duration{item.at("duration_ms").get<time::Duration::rep>()},
        item.value("magnitude", 0.0),
        item.value("seed", 0U),
    });
  }

  const auto& expected = root.at("expected");
  scenario.expected.detection_max =
      time::Duration{expected.at("detection_max_ms").get<time::Duration::rep>()};
  scenario.expected.action = action_from_string(expected.at("action").get<std::string>());
  scenario.expected.recovery = expected.at("recovery").get<bool>();
  validate(scenario);
  return scenario;
}

void ScenarioLoader::validate(const Scenario& scenario) {
  if (scenario.schema_version != 1) throw std::invalid_argument("unsupported schema version");
  if (scenario.id.empty() || scenario.name.empty()) {
    throw std::invalid_argument("scenario id and name are required");
  }
  if (scenario.run_duration <= time::Duration{0}) {
    throw std::invalid_argument("run duration must be positive");
  }
  if (scenario.faults.empty()) throw std::invalid_argument("scenario requires at least one fault");
  if (scenario.expected.detection_max < time::Duration{0}) {
    throw std::invalid_argument("detection deadline cannot be negative");
  }

  std::set<std::string> ids;
  for (const auto& fault : scenario.faults) {
    if (!ids.insert(fault.id).second) throw std::invalid_argument("duplicate fault id");
    if (fault.id.empty() || fault.duration <= time::Duration{0} ||
        fault.start < time::Duration{0} ||
        fault.start + fault.duration > scenario.run_duration) {
      throw std::invalid_argument("fault interval is outside scenario duration");
    }
  }
}

}  // namespace uavmv::simulation
