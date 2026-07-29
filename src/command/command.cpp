#include "uavmv/command/command.hpp"

namespace uavmv::command {

std::string_view to_string(CommandType command) {
  switch (command) {
    case CommandType::Arm: return "ARM";
    case CommandType::Disarm: return "DISARM";
    case CommandType::Takeoff: return "TAKEOFF";
    case CommandType::StartMission: return "START_MISSION";
    case CommandType::Hold: return "HOLD";
    case CommandType::ReturnToHome: return "RETURN_TO_HOME";
    case CommandType::Land: return "LAND";
    case CommandType::Abort: return "ABORT";
  }
  return "UNKNOWN_COMMAND";
}

}  // namespace uavmv::command
