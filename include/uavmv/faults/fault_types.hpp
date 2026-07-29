#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "uavmv/time/clock.hpp"

namespace uavmv::faults {

enum class FaultType {
  TelemetryDelay,
  TelemetryPacketLoss,
  TelemetryLoss,
  GpsDegradation,
  GpsDropout,
  CommandRejection,
  CommandAckTimeout,
  MissionMismatch,
  OffboardHeartbeatLoss,
  BatteryCritical,
  TelemetryAndGpsDegradation,
  TransientTelemetryLoss
};

enum class FaultSeverity { Low, Medium, High, Critical };

struct FaultDefinition {
  std::string id;
  std::string name;
  FaultType type;
  FaultSeverity severity;
  time::Duration start;
  time::Duration duration;
  double magnitude{0.0};
  std::uint32_t seed{0};
};

[[nodiscard]] std::string_view to_string(FaultType type);
[[nodiscard]] std::string_view to_string(FaultSeverity severity);
[[nodiscard]] FaultType fault_type_from_string(std::string_view value);
[[nodiscard]] FaultSeverity fault_severity_from_string(std::string_view value);

}  // namespace uavmv::faults
