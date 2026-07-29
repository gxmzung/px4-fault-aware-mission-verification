#include "uavmv/failsafe/failsafe_supervisor.hpp"

#include <algorithm>
#include <string>

namespace uavmv::failsafe {

std::string_view to_string(FailsafeAction action) {
  switch (action) {
    case FailsafeAction::None: return "NONE";
    case FailsafeAction::Hold: return "HOLD";
    case FailsafeAction::ReturnToHome: return "RETURN_TO_HOME";
    case FailsafeAction::Land: return "LAND";
    case FailsafeAction::ResumeMission: return "RESUME_MISSION";
  }
  return "UNKNOWN_ACTION";
}

FailsafeSupervisor::FailsafeSupervisor(const time::IClock& clock, FailsafePolicy policy,
                                       logging::EventLogger& logger)
    : clock_(clock), policy_(policy), logger_(logger) {}

FailsafeDecision FailsafeSupervisor::evaluate(const telemetry::HealthAssessment& assessment) {
  FailsafeAction decision = FailsafeAction::None;
  bool recovery = false;

  switch (assessment.overall) {
    case telemetry::HealthState::Healthy:
      if (active_action_ != FailsafeAction::None) {
        recovery = true;
        ++healthy_streak_;
        if (healthy_streak_ >= std::max(1U, policy_.healthy_samples_for_resume)) {
          decision = FailsafeAction::ResumeMission;
          active_action_ = FailsafeAction::None;
          healthy_streak_ = 0;
        } else {
          decision = active_action_;
        }
      }
      break;
    case telemetry::HealthState::Degraded:
      healthy_streak_ = 0;
      decision = policy_.degraded_action;
      active_action_ = decision;
      break;
    case telemetry::HealthState::Critical:
      healthy_streak_ = 0;
      decision = policy_.critical_action;
      active_action_ = decision;
      break;
    case telemetry::HealthState::Lost:
      healthy_streak_ = 0;
      decision = policy_.lost_action;
      active_action_ = decision;
      break;
  }

  logger_.append({clock_.now(), logging::RecordKind::FailsafeDecision, next_record_id_++,
                  mission::MissionState::Boot, mission::MissionState::Boot,
                  std::string(to_string(decision)), true, mission::ReasonCode::None});
  return {decision, assessment.overall, recovery};
}

FailsafeAction FailsafeSupervisor::active_action() const { return active_action_; }

}  // namespace uavmv::failsafe
