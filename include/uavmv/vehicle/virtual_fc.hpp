#pragma once

#include <optional>

#include "uavmv/vehicle/vehicle_adapter.hpp"

namespace uavmv::vehicle {

class VirtualFC final : public IVehicleAdapter {
 public:
  command::CommandAcknowledgement execute(const command::Command& command) override;
  [[nodiscard]] VehicleState state() const override;

  void set_next_command_result(bool accepted);

 private:
  void apply(const command::Command& command);

  VehicleState state_;
  std::optional<bool> next_result_;
};

}  // namespace uavmv::vehicle
