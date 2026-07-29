#pragma once

#include <vector>

#include "uavmv/faults/fault_types.hpp"
#include "uavmv/telemetry/telemetry_snapshot.hpp"

namespace uavmv::faults {

struct FaultEffects {
  telemetry::TelemetrySnapshot telemetry;
  bool reject_next_command{false};
};

class FaultInjector {
 public:
  [[nodiscard]] FaultEffects apply(const telemetry::TelemetrySnapshot& input,
                                   const std::vector<FaultDefinition>& active_faults,
                                   std::uint64_t sample_index) const;
};

}  // namespace uavmv::faults
