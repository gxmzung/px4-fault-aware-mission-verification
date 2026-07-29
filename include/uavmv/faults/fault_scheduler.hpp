#pragma once

#include <vector>

#include "uavmv/faults/fault_types.hpp"
#include "uavmv/time/clock.hpp"

namespace uavmv::faults {

class FaultScheduler {
 public:
  explicit FaultScheduler(std::vector<FaultDefinition> faults);

  [[nodiscard]] std::vector<FaultDefinition> active_at(time::TimePoint time) const;
  [[nodiscard]] const std::vector<FaultDefinition>& definitions() const;

 private:
  std::vector<FaultDefinition> faults_;
};

}  // namespace uavmv::faults
