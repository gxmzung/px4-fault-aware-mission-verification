#pragma once

#include "uavmv/command/command.hpp"
#include "uavmv/mission/mission_state.hpp"

namespace uavmv::command {

struct CommandDecision {
  Command command;
  mission::MissionState state;
  bool allowed;
  mission::ReasonCode reason;
};

struct CommandAcknowledgement {
  Command command;
  bool accepted;
  mission::ReasonCode reason;
};

}  // namespace uavmv::command
