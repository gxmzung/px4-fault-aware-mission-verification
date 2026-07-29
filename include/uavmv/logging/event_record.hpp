#pragma once

#include <cstdint>
#include <string>

#include "uavmv/mission/mission_state.hpp"
#include "uavmv/time/clock.hpp"

namespace uavmv::logging {

enum class RecordKind {
  MissionTransition,
  CommandDecision,
  VehicleCommand,
  HealthAssessment,
  FailsafeDecision
};

struct EventRecord {
  time::TimePoint timestamp{};
  RecordKind kind{RecordKind::MissionTransition};
  std::uint64_t record_id{0};
  mission::MissionState previous_state{mission::MissionState::Boot};
  mission::MissionState next_state{mission::MissionState::Boot};
  std::string subject;
  bool accepted{false};
  mission::ReasonCode reason{mission::ReasonCode::None};
};

[[nodiscard]] std::string_view to_string(RecordKind kind);
[[nodiscard]] std::string to_json(const EventRecord& record);

}  // namespace uavmv::logging
