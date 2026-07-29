#pragma once

#include <filesystem>

#include "uavmv/simulation/scenario.hpp"

namespace uavmv::simulation {

class ScenarioLoader {
 public:
  [[nodiscard]] static Scenario load_json(const std::filesystem::path& path);
  static void validate(const Scenario& scenario);
};

}  // namespace uavmv::simulation
