#pragma once

#include <cstdint>
#include <string_view>

#include "uavmv/time/clock.hpp"

namespace uavmv::command {

enum class CommandType {
  Arm,
  Disarm,
  Takeoff,
  StartMission,
  Hold,
  ReturnToHome,
  Land,
  Abort
};

struct Command {
  CommandType type;
  std::uint64_t sequence;
  time::TimePoint issued_at;
};

[[nodiscard]] std::string_view to_string(CommandType command);

}  // namespace uavmv::command
