#include "telemetry_health.hpp"

bool TelemetryHealth::isBatterySafe() const {
    return batteryPercent >= 25.0;
}

bool TelemetryHealth::isSignalSafe() const {
    return signalStrength >= 40.0;
}

bool TelemetryHealth::isGpsSafe() const {
    return gpsSatellites >= 8.0;
}

bool TelemetryHealth::isTemperatureSafe() const {
    return temperature <= 75.0;
}

bool TelemetryHealth::isMissionSafe() const {
    return isBatterySafe() &&
           isSignalSafe() &&
           isGpsSafe() &&
           isTemperatureSafe();
}