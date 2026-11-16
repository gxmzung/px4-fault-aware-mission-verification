#ifndef TELEMETRY_HEALTH_HPP
#define TELEMETRY_HEALTH_HPP

struct TelemetryHealth {
    double batteryPercent;
    double signalStrength;
    double gpsSatellites;
    double temperature;

    bool isBatterySafe() const;
    bool isSignalSafe() const;
    bool isGpsSafe() const;
    bool isTemperatureSafe() const;
    bool isMissionSafe() const;
};

#endif