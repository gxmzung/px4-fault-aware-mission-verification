#pragma once

#include <string_view>

namespace uavmv::failsafe {

enum class FailsafeAction { None, Hold, ReturnToHome, Land, ResumeMission };

struct FailsafePolicy {
  FailsafeAction degraded_action{FailsafeAction::Hold};
  FailsafeAction critical_action{FailsafeAction::ReturnToHome};
  FailsafeAction lost_action{FailsafeAction::Land};
  unsigned healthy_samples_for_resume{2};
};

[[nodiscard]] std::string_view to_string(FailsafeAction action);

}  // namespace uavmv::failsafe
