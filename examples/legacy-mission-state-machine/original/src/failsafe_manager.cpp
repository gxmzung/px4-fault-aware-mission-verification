#include "failsafe_manager.hpp"

bool FailsafeManager::shouldTriggerFailsafe(const TelemetryHealth& health) const {
    return !health.isMissionSafe();
}

std::string FailsafeManager::getFailsafeReason(const TelemetryHealth& health) const {
    if (!health.isBatterySafe()) {
        return "LOW_BATTERY";
    }

    if (!health.isSignalSafe()) {
        return "LOW_SIGNAL";
    }

    if (!health.isGpsSafe()) {
        return "GPS_UNSAFE";
    }

    if (!health.isTemperatureSafe()) {
        return "HIGH_TEMPERATURE";
    }

    return "MISSION_SAFE";
}