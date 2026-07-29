#pragma once

#include <string_view>

namespace uavmv::mission {

enum class MissionState {
  Boot,
  Preflight,
  Armed,
  Takeoff,
  Mission,
  Hold,
  ReturnToHome,
  Land,
  Recovery,
  Failsafe,
  Completed,
  Aborted
};

enum class MissionEvent {
  SystemReady,
  PreflightOk,
  ArmConfirmed,
  TakeoffConfirmed,
  WaypointReached,
  MissionComplete,
  HoldRequested,
  RthRequested,
  LandRequested,
  RecoverySuccess,
  RecoveryFailed,
  OperatorAbort
};

enum class ReasonCode {
  None,
  InvalidStateTransition,
  PreflightNotReady,
  VehicleNotArmed,
  MissionNotLoaded,
  DuplicateCommand,
  TerminalMissionState,
  CommandRejected,
  OperatorAbort
};

[[nodiscard]] std::string_view to_string(MissionState state);
[[nodiscard]] std::string_view to_string(MissionEvent event);
[[nodiscard]] std::string_view to_string(ReasonCode reason);
[[nodiscard]] bool is_terminal(MissionState state);

}  // namespace uavmv::mission
