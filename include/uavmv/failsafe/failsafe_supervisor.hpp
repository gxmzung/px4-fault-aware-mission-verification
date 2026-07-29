#pragma once

#include "uavmv/failsafe/failsafe_policy.hpp"
#include "uavmv/logging/event_logger.hpp"
#include "uavmv/telemetry/telemetry_health.hpp"
#include "uavmv/time/clock.hpp"

namespace uavmv::failsafe {

struct FailsafeDecision {
  FailsafeAction action;
  telemetry::HealthState health;
  bool recovery;
};

class FailsafeSupervisor {
 public:
  FailsafeSupervisor(const time::IClock& clock, FailsafePolicy policy,
                     logging::EventLogger& logger);

  FailsafeDecision evaluate(const telemetry::HealthAssessment& assessment);
  [[nodiscard]] FailsafeAction active_action() const;

 private:
  const time::IClock& clock_;
  FailsafePolicy policy_;
  logging::EventLogger& logger_;
  FailsafeAction active_action_{FailsafeAction::None};
  unsigned healthy_streak_{0};
  std::uint64_t next_record_id_{1};
};

}  // namespace uavmv::failsafe
