#pragma once

#include "uavmv/time/clock.hpp"

namespace uavmv::telemetry {

struct FreshnessThresholds {
  time::Duration healthy_max{500};
  time::Duration degraded_max{1000};
  time::Duration critical_max{2000};
};

struct TelemetryRules {
  FreshnessThresholds heartbeat{};
  FreshnessThresholds gps{time::Duration{1000}, time::Duration{2000},
                          time::Duration{5000}};
  FreshnessThresholds position{time::Duration{500}, time::Duration{1000},
                               time::Duration{2000}};
  FreshnessThresholds attitude{time::Duration{250}, time::Duration{500},
                               time::Duration{1000}};
  FreshnessThresholds command_ack{time::Duration{1000}, time::Duration{2000},
                                  time::Duration{5000}};
  FreshnessThresholds mission_sync{time::Duration{1000}, time::Duration{2000},
                                   time::Duration{5000}};
  double battery_degraded_percent{30.0};
  double battery_critical_percent{15.0};
  double battery_lost_percent{5.0};
  unsigned healthy_samples_for_recovery{2};
};

}  // namespace uavmv::telemetry
