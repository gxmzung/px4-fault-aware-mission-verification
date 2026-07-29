#include "uavmv/faults/fault_injector.hpp"

#include <algorithm>

namespace uavmv::faults {
namespace {

void delay(telemetry::TimedValidity& signal, double magnitude_ms) {
  signal.updated_at -= time::Duration{static_cast<time::Duration::rep>(magnitude_ms)};
}

void lose_vehicle_telemetry(telemetry::TelemetrySnapshot& snapshot) {
  snapshot.heartbeat.valid = false;
  snapshot.position.valid = false;
  snapshot.attitude.valid = false;
  snapshot.command_ack.valid = false;
  snapshot.mission_sync.valid = false;
}

}  // namespace

FaultEffects FaultInjector::apply(const telemetry::TelemetrySnapshot& input,
                                  const std::vector<FaultDefinition>& active_faults,
                                  std::uint64_t sample_index) const {
  FaultEffects effects{input, false};
  for (const auto& fault : active_faults) {
    switch (fault.type) {
      case FaultType::TelemetryDelay:
        delay(effects.telemetry.heartbeat, fault.magnitude);
        delay(effects.telemetry.position, fault.magnitude);
        delay(effects.telemetry.attitude, fault.magnitude);
        break;
      case FaultType::TelemetryPacketLoss: {
        const auto percentage = std::clamp(fault.magnitude, 0.0, 100.0);
        const auto bucket = static_cast<double>((sample_index + fault.seed) % 100);
        if (bucket < percentage) effects.telemetry.heartbeat.valid = false;
        break;
      }
      case FaultType::TelemetryLoss:
      case FaultType::TransientTelemetryLoss:
        lose_vehicle_telemetry(effects.telemetry);
        break;
      case FaultType::GpsDegradation:
        delay(effects.telemetry.gps, fault.magnitude);
        break;
      case FaultType::GpsDropout:
        effects.telemetry.gps.valid = false;
        break;
      case FaultType::CommandRejection:
        effects.reject_next_command = true;
        break;
      case FaultType::CommandAckTimeout:
        effects.telemetry.command_ack.valid = false;
        break;
      case FaultType::MissionMismatch:
        effects.telemetry.mission_sync.valid = false;
        break;
      case FaultType::OffboardHeartbeatLoss:
        effects.telemetry.offboard_heartbeat.valid = false;
        break;
      case FaultType::BatteryCritical:
        effects.telemetry.battery_percent = fault.magnitude > 0.0 ? fault.magnitude : 10.0;
        break;
      case FaultType::TelemetryAndGpsDegradation:
        delay(effects.telemetry.heartbeat, fault.magnitude);
        delay(effects.telemetry.position, fault.magnitude);
        effects.telemetry.gps.valid = false;
        break;
    }
  }
  return effects;
}

}  // namespace uavmv::faults
