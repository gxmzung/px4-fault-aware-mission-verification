#pragma once

#include "uavmv/mission/mission_state.hpp"

namespace uavmv::mission {

struct MissionEventRecord {
  MissionEvent event;
  ReasonCode reason{ReasonCode::None};
};

}  // namespace uavmv::mission
