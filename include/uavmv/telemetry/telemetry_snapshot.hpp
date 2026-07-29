#pragma once

#include "uavmv/time/clock.hpp"

namespace uavmv::telemetry {

struct TimedValidity {
  time::TimePoint updated_at{};
  bool valid{true};
};

struct TelemetrySnapshot {
  TimedValidity heartbeat;
  TimedValidity gps;
  TimedValidity position;
  TimedValidity attitude;
  TimedValidity command_ack;
  TimedValidity mission_sync;
  double battery_percent{100.0};
};

}  // namespace uavmv::telemetry
