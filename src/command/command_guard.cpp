#include "uavmv/command/command_guard.hpp"

namespace uavmv::command {
namespace {

mission::ReasonCode validate(const Command& command, const mission::MissionContext& context) {
  using mission::MissionState;
  using mission::ReasonCode;

  if (mission::is_terminal(context.state)) return ReasonCode::TerminalMissionState;
  if (context.last_command_sequence && command.sequence <= *context.last_command_sequence) {
    return ReasonCode::DuplicateCommand;
  }

  switch (command.type) {
    case CommandType::Arm:
      if (context.state != MissionState::Preflight) return ReasonCode::InvalidStateTransition;
      return context.preflight_ready ? ReasonCode::None : ReasonCode::PreflightNotReady;
    case CommandType::Disarm:
      if (!context.armed) return ReasonCode::VehicleNotArmed;
      return (context.state == MissionState::Land || context.state == MissionState::Aborted)
                 ? ReasonCode::None
                 : ReasonCode::InvalidStateTransition;
    case CommandType::Takeoff:
      if (context.state != MissionState::Armed) return ReasonCode::InvalidStateTransition;
      return context.armed ? ReasonCode::None : ReasonCode::VehicleNotArmed;
    case CommandType::StartMission:
      if (context.state != MissionState::Takeoff) return ReasonCode::InvalidStateTransition;
      return context.mission_loaded ? ReasonCode::None : ReasonCode::MissionNotLoaded;
    case CommandType::Hold:
      return context.state == MissionState::Mission ? ReasonCode::None
                                                    : ReasonCode::InvalidStateTransition;
    case CommandType::ReturnToHome:
      return (context.state == MissionState::Takeoff || context.state == MissionState::Mission ||
              context.state == MissionState::Hold)
                 ? ReasonCode::None
                 : ReasonCode::InvalidStateTransition;
    case CommandType::Land:
      return (context.state == MissionState::Armed || context.state == MissionState::Takeoff ||
              context.state == MissionState::Mission || context.state == MissionState::Hold ||
              context.state == MissionState::ReturnToHome ||
              context.state == MissionState::Failsafe)
                 ? ReasonCode::None
                 : ReasonCode::InvalidStateTransition;
    case CommandType::Abort:
      return ReasonCode::None;
  }
  return ReasonCode::InvalidStateTransition;
}

}  // namespace

CommandGuard::CommandGuard(const time::IClock& clock, logging::EventLogger& logger)
    : clock_(clock), logger_(logger) {}

CommandDecision CommandGuard::evaluate(const Command& command,
                                       const mission::MissionContext& context) {
  const auto reason = validate(command, context);
  const bool allowed = reason == mission::ReasonCode::None;
  logger_.append({clock_.now(), logging::RecordKind::CommandDecision, next_record_id_++,
                  context.state, context.state, std::string(to_string(command.type)), allowed,
                  reason});
  return {command, context.state, allowed, reason};
}

}  // namespace uavmv::command
