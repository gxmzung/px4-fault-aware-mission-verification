#pragma once

#include "uavmv/command/command_result.hpp"
#include "uavmv/logging/event_logger.hpp"
#include "uavmv/mission/mission_context.hpp"
#include "uavmv/time/clock.hpp"

namespace uavmv::command {

class CommandGuard {
 public:
  CommandGuard(const time::IClock& clock, logging::EventLogger& logger);

  CommandDecision evaluate(const Command& command,
                           const mission::MissionContext& context);

 private:
  const time::IClock& clock_;
  logging::EventLogger& logger_;
  std::uint64_t next_record_id_{1};
};

}  // namespace uavmv::command
