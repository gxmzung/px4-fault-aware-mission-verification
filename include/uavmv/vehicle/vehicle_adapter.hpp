#pragma once

#include "uavmv/command/command_result.hpp"

namespace uavmv::vehicle {

enum class VehicleMode { Disarmed, Armed, TakingOff, Mission, Hold, ReturnToHome, Landing, Aborted };

struct VehicleState {
  bool armed{false};
  VehicleMode mode{VehicleMode::Disarmed};
};

class IVehicleAdapter {
 public:
  virtual ~IVehicleAdapter() = default;
  virtual command::CommandAcknowledgement execute(const command::Command& command) = 0;
  [[nodiscard]] virtual VehicleState state() const = 0;
};

}  // namespace uavmv::vehicle
