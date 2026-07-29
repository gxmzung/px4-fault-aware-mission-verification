#pragma once

#include <cstdint>
#include <optional>

#include "uavmv/mission/mission_state.hpp"

namespace uavmv::mission {

struct MissionContext {
  MissionState state{MissionState::Boot};
  bool preflight_ready{false};
  bool armed{false};
  bool mission_loaded{false};
  bool operator_abort{false};
  std::optional<std::uint64_t> last_command_sequence;
};

}  // namespace uavmv::mission
