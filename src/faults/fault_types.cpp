#include "uavmv/faults/fault_types.hpp"

#include <stdexcept>

namespace uavmv::faults {

std::string_view to_string(FaultType type) {
  switch (type) {
    case FaultType::TelemetryDelay: return "telemetry_delay";
    case FaultType::TelemetryPacketLoss: return "telemetry_packet_loss";
    case FaultType::TelemetryLoss: return "telemetry_loss";
    case FaultType::GpsDegradation: return "gps_degradation";
    case FaultType::GpsDropout: return "gps_dropout";
    case FaultType::CommandRejection: return "command_rejection";
    case FaultType::CommandAckTimeout: return "command_ack_timeout";
    case FaultType::MissionMismatch: return "mission_mismatch";
    case FaultType::OffboardHeartbeatLoss: return "offboard_heartbeat_loss";
    case FaultType::BatteryCritical: return "battery_critical";
    case FaultType::TelemetryAndGpsDegradation: return "telemetry_and_gps_degradation";
    case FaultType::TransientTelemetryLoss: return "transient_telemetry_loss";
  }
  return "unknown_fault";
}

std::string_view to_string(FaultSeverity severity) {
  switch (severity) {
    case FaultSeverity::Low: return "low";
    case FaultSeverity::Medium: return "medium";
    case FaultSeverity::High: return "high";
    case FaultSeverity::Critical: return "critical";
  }
  return "unknown_severity";
}

FaultType fault_type_from_string(std::string_view value) {
  for (const auto type : {FaultType::TelemetryDelay, FaultType::TelemetryPacketLoss,
                          FaultType::TelemetryLoss, FaultType::GpsDegradation,
                          FaultType::GpsDropout, FaultType::CommandRejection,
                          FaultType::CommandAckTimeout, FaultType::MissionMismatch,
                          FaultType::OffboardHeartbeatLoss, FaultType::BatteryCritical,
                          FaultType::TelemetryAndGpsDegradation,
                          FaultType::TransientTelemetryLoss}) {
    if (to_string(type) == value) return type;
  }
  throw std::invalid_argument("unknown fault type: " + std::string(value));
}

FaultSeverity fault_severity_from_string(std::string_view value) {
  for (const auto severity : {FaultSeverity::Low, FaultSeverity::Medium, FaultSeverity::High,
                              FaultSeverity::Critical}) {
    if (to_string(severity) == value) return severity;
  }
  throw std::invalid_argument("unknown fault severity: " + std::string(value));
}

}  // namespace uavmv::faults
