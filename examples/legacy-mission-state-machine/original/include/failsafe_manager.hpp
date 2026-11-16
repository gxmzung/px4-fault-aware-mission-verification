#ifndef FAILSAFE_MANAGER_HPP
#define FAILSAFE_MANAGER_HPP

#include <string>
#include "telemetry_health.hpp"

class FailsafeManager {
public:
    bool shouldTriggerFailsafe(const TelemetryHealth& health) const;
    std::string getFailsafeReason(const TelemetryHealth& health) const;
};

#endif