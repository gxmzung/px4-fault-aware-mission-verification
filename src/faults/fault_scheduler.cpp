#include "uavmv/faults/fault_scheduler.hpp"

#include <algorithm>
#include <stdexcept>

namespace uavmv::faults {

FaultScheduler::FaultScheduler(std::vector<FaultDefinition> faults) : faults_(std::move(faults)) {
  for (const auto& fault : faults_) {
    if (fault.id.empty() || fault.duration <= time::Duration{0} ||
        fault.start < time::Duration{0}) {
      throw std::invalid_argument("fault requires id, non-negative start, and positive duration");
    }
  }
  std::stable_sort(faults_.begin(), faults_.end(),
                   [](const auto& left, const auto& right) { return left.start < right.start; });
}

std::vector<FaultDefinition> FaultScheduler::active_at(time::TimePoint time) const {
  std::vector<FaultDefinition> active;
  const auto elapsed = time.time_since_epoch();
  for (const auto& fault : faults_) {
    if (elapsed >= fault.start && elapsed < fault.start + fault.duration) {
      active.push_back(fault);
    }
  }
  return active;
}

const std::vector<FaultDefinition>& FaultScheduler::definitions() const { return faults_; }

}  // namespace uavmv::faults
