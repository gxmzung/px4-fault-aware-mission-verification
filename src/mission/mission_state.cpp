#include "uavmv/mission/mission_state.hpp"

namespace uavmv::mission {

std::string_view to_string(MissionState state) {
  switch (state) {
    case MissionState::Boot: return "BOOT";
    case MissionState::Preflight: return "PREFLIGHT";
    case MissionState::Armed: return "ARMED";
    case MissionState::Takeoff: return "TAKEOFF";
    case MissionState::Mission: return "MISSION";
    case MissionState::Hold: return "HOLD";
    case MissionState::ReturnToHome: return "RETURN_TO_HOME";
    case MissionState::Land: return "LAND";
    case MissionState::Recovery: return "RECOVERY";
    case MissionState::Failsafe: return "FAILSAFE";
    case MissionState::Completed: return "COMPLETED";
    case MissionState::Aborted: return "ABORTED";
  }
  return "UNKNOWN_STATE";
}

std::string_view to_string(MissionEvent event) {
  switch (event) {
    case MissionEvent::SystemReady: return "SYSTEM_READY";
    case MissionEvent::PreflightOk: return "PREFLIGHT_OK";
    case MissionEvent::ArmConfirmed: return "ARM_CONFIRMED";
    case MissionEvent::TakeoffConfirmed: return "TAKEOFF_CONFIRMED";
    case MissionEvent::WaypointReached: return "WAYPOINT_REACHED";
    case MissionEvent::MissionComplete: return "MISSION_COMPLETE";
    case MissionEvent::HoldRequested: return "HOLD_REQUESTED";
    case MissionEvent::RthRequested: return "RTH_REQUESTED";
    case MissionEvent::LandRequested: return "LAND_REQUESTED";
    case MissionEvent::RecoverySuccess: return "RECOVERY_SUCCESS";
    case MissionEvent::RecoveryFailed: return "RECOVERY_FAILED";
    case MissionEvent::OperatorAbort: return "OPERATOR_ABORT";
  }
  return "UNKNOWN_EVENT";
}

std::string_view to_string(ReasonCode reason) {
  switch (reason) {
    case ReasonCode::None: return "NONE";
    case ReasonCode::InvalidStateTransition: return "INVALID_STATE_TRANSITION";
    case ReasonCode::PreflightNotReady: return "PREFLIGHT_NOT_READY";
    case ReasonCode::VehicleNotArmed: return "VEHICLE_NOT_ARMED";
    case ReasonCode::MissionNotLoaded: return "MISSION_NOT_LOADED";
    case ReasonCode::DuplicateCommand: return "DUPLICATE_COMMAND";
    case ReasonCode::TerminalMissionState: return "TERMINAL_MISSION_STATE";
    case ReasonCode::CommandRejected: return "COMMAND_REJECTED";
    case ReasonCode::OperatorAbort: return "OPERATOR_ABORT";
  }
  return "UNKNOWN_REASON";
}

bool is_terminal(MissionState state) {
  return state == MissionState::Completed || state == MissionState::Aborted;
}

}  // namespace uavmv::mission
