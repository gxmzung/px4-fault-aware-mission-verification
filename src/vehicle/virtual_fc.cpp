#include "uavmv/vehicle/virtual_fc.hpp"

namespace uavmv::vehicle {

command::CommandAcknowledgement VirtualFC::execute(const command::Command& command) {
  const bool accepted = next_result_.value_or(true);
  next_result_.reset();
  if (accepted) apply(command);
  return {command, accepted,
          accepted ? mission::ReasonCode::None : mission::ReasonCode::CommandRejected};
}

VehicleState VirtualFC::state() const { return state_; }

void VirtualFC::set_next_command_result(bool accepted) { next_result_ = accepted; }

void VirtualFC::apply(const command::Command& command) {
  switch (command.type) {
    case command::CommandType::Arm:
      state_ = {true, VehicleMode::Armed};
      break;
    case command::CommandType::Disarm:
      state_ = {false, VehicleMode::Disarmed};
      break;
    case command::CommandType::Takeoff:
      state_.mode = VehicleMode::TakingOff;
      break;
    case command::CommandType::StartMission:
      state_.mode = VehicleMode::Mission;
      break;
    case command::CommandType::Hold:
      state_.mode = VehicleMode::Hold;
      break;
    case command::CommandType::ReturnToHome:
      state_.mode = VehicleMode::ReturnToHome;
      break;
    case command::CommandType::Land:
      state_.mode = VehicleMode::Landing;
      break;
    case command::CommandType::Abort:
      state_ = {false, VehicleMode::Aborted};
      break;
  }
}

}  // namespace uavmv::vehicle
