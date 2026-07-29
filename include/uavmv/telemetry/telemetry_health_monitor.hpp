#pragma once

#include "uavmv/logging/event_logger.hpp"
#include "uavmv/telemetry/telemetry_health.hpp"
#include "uavmv/telemetry/telemetry_rules.hpp"
#include "uavmv/telemetry/telemetry_snapshot.hpp"
#include "uavmv/time/clock.hpp"

namespace uavmv::telemetry {

class TelemetryHealthMonitor {
 public:
  TelemetryHealthMonitor(const time::IClock& clock, TelemetryRules rules,
                         logging::EventLogger& logger);

  HealthAssessment evaluate(const TelemetrySnapshot& snapshot);
  [[nodiscard]] HealthState state() const;

 private:
  const time::IClock& clock_;
  TelemetryRules rules_;
  logging::EventLogger& logger_;
  HealthState state_{HealthState::Healthy};
  unsigned healthy_streak_{0};
  std::uint64_t next_record_id_{1};
};

}  // namespace uavmv::telemetry
