#pragma once

#include "uavmv/logging/event_logger.hpp"
#include "uavmv/mission/mission_context.hpp"
#include "uavmv/time/clock.hpp"

namespace uavmv::mission {

struct TransitionResult {
  MissionState previous_state;
  MissionState next_state;
  MissionEvent event;
  bool accepted;
  ReasonCode reason;
};

class MissionStateMachine {
 public:
  MissionStateMachine(MissionContext& context, const time::IClock& clock,
                      logging::EventLogger& logger);

  [[nodiscard]] MissionState state() const;
  TransitionResult handle(MissionEvent event);

 private:
  MissionContext& context_;
  const time::IClock& clock_;
  logging::EventLogger& logger_;
  std::uint64_t next_record_id_{1};
};

}  // namespace uavmv::mission
