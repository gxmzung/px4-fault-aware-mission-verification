#pragma once

#include <string>
#include <vector>

#include "uavmv/failsafe/failsafe_policy.hpp"
#include "uavmv/faults/fault_types.hpp"
#include "uavmv/time/clock.hpp"

namespace uavmv::simulation {

struct ExpectedBehavior {
  time::Duration detection_max{1000};
  failsafe::FailsafeAction action{failsafe::FailsafeAction::None};
  bool recovery{false};
};

struct Scenario {
  int schema_version{1};
  std::string id;
  std::string name;
  time::Duration run_duration{10000};
  std::vector<faults::FaultDefinition> faults;
  ExpectedBehavior expected;
};

}  // namespace uavmv::simulation
