#include "uavmv/mission/mission_state_machine.hpp"

#include <array>
#include <optional>

namespace uavmv::mission {
namespace {

struct TransitionRule {
  MissionState from;
  MissionEvent event;
  MissionState to;
};

constexpr std::array kRules{
    TransitionRule{MissionState::Boot, MissionEvent::SystemReady, MissionState::Preflight},
    TransitionRule{MissionState::Preflight, MissionEvent::PreflightOk, MissionState::Preflight},
    TransitionRule{MissionState::Preflight, MissionEvent::ArmConfirmed, MissionState::Armed},
    TransitionRule{MissionState::Armed, MissionEvent::TakeoffConfirmed, MissionState::Takeoff},
    TransitionRule{MissionState::Takeoff, MissionEvent::WaypointReached, MissionState::Mission},
    TransitionRule{MissionState::Mission, MissionEvent::WaypointReached, MissionState::Mission},
    TransitionRule{MissionState::Mission, MissionEvent::MissionComplete, MissionState::ReturnToHome},
    TransitionRule{MissionState::Mission, MissionEvent::HoldRequested, MissionState::Hold},
    TransitionRule{MissionState::Hold, MissionEvent::WaypointReached, MissionState::Mission},
    TransitionRule{MissionState::Takeoff, MissionEvent::RthRequested, MissionState::ReturnToHome},
    TransitionRule{MissionState::Mission, MissionEvent::RthRequested, MissionState::ReturnToHome},
    TransitionRule{MissionState::Hold, MissionEvent::RthRequested, MissionState::ReturnToHome},
    TransitionRule{MissionState::Armed, MissionEvent::LandRequested, MissionState::Land},
    TransitionRule{MissionState::Takeoff, MissionEvent::LandRequested, MissionState::Land},
    TransitionRule{MissionState::Mission, MissionEvent::LandRequested, MissionState::Land},
    TransitionRule{MissionState::Hold, MissionEvent::LandRequested, MissionState::Land},
    TransitionRule{MissionState::ReturnToHome, MissionEvent::LandRequested, MissionState::Land},
    TransitionRule{MissionState::Land, MissionEvent::MissionComplete, MissionState::Completed},
    TransitionRule{MissionState::Recovery, MissionEvent::RecoverySuccess, MissionState::Hold},
    TransitionRule{MissionState::Recovery, MissionEvent::RecoveryFailed, MissionState::Failsafe},
    TransitionRule{MissionState::Failsafe, MissionEvent::LandRequested, MissionState::Land},
};

std::optional<MissionState> find_transition(MissionState from, MissionEvent event) {
  for (const auto& rule : kRules) {
    if (rule.from == from && rule.event == event) {
      return rule.to;
    }
  }
  return std::nullopt;
}

}  // namespace

MissionStateMachine::MissionStateMachine(MissionContext& context, const time::IClock& clock,
                                         logging::EventLogger& logger)
    : context_(context), clock_(clock), logger_(logger) {}

MissionState MissionStateMachine::state() const { return context_.state; }

TransitionResult MissionStateMachine::handle(MissionEvent event) {
  const MissionState previous = context_.state;
  bool accepted = false;
  ReasonCode reason = ReasonCode::InvalidStateTransition;
  MissionState next = previous;

  if (event == MissionEvent::OperatorAbort && !is_terminal(previous)) {
    accepted = true;
    reason = ReasonCode::OperatorAbort;
    next = MissionState::Aborted;
    context_.operator_abort = true;
    context_.armed = false;
  } else if (const auto target = find_transition(previous, event)) {
    if (event == MissionEvent::ArmConfirmed && !context_.preflight_ready) {
      reason = ReasonCode::PreflightNotReady;
    } else {
      accepted = true;
      reason = ReasonCode::None;
      next = *target;
      if (event == MissionEvent::PreflightOk) context_.preflight_ready = true;
      if (event == MissionEvent::ArmConfirmed) context_.armed = true;
      if (next == MissionState::Completed) context_.armed = false;
    }
  } else if (is_terminal(previous)) {
    reason = ReasonCode::TerminalMissionState;
  }

  if (accepted) context_.state = next;

  logger_.append({clock_.now(), logging::RecordKind::MissionTransition, next_record_id_++,
                  previous, context_.state, std::string(to_string(event)), accepted, reason});
  return {previous, context_.state, event, accepted, reason};
}

}  // namespace uavmv::mission
