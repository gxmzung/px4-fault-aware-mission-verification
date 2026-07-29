#pragma once

#include <array>
#include <string_view>

namespace uavmv::telemetry {

enum class HealthState { Healthy, Degraded, Critical, Lost };
enum class TelemetrySignal {
  Heartbeat,
  Gps,
  Position,
  Attitude,
  Battery,
  CommandAck,
  MissionSync
};

struct SignalHealth {
  TelemetrySignal signal;
  HealthState state;
};

struct HealthAssessment {
  std::array<SignalHealth, 7> signals;
  HealthState overall;
};

[[nodiscard]] std::string_view to_string(HealthState state);
[[nodiscard]] std::string_view to_string(TelemetrySignal signal);
[[nodiscard]] int severity(HealthState state);

}  // namespace uavmv::telemetry
