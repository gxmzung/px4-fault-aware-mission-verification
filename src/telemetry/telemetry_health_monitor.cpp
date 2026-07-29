#include "uavmv/telemetry/telemetry_health_monitor.hpp"

#include <algorithm>
#include <string>

namespace uavmv::telemetry {
namespace {

HealthState freshness_state(time::TimePoint now, const TimedValidity& sample,
                            const FreshnessThresholds& thresholds) {
  if (!sample.valid) return HealthState::Lost;
  const auto age = now > sample.updated_at ? now - sample.updated_at : time::Duration{0};
  if (age <= thresholds.healthy_max) return HealthState::Healthy;
  if (age <= thresholds.degraded_max) return HealthState::Degraded;
  if (age <= thresholds.critical_max) return HealthState::Critical;
  return HealthState::Lost;
}

HealthState battery_state(double percent, const TelemetryRules& rules) {
  if (percent <= rules.battery_lost_percent) return HealthState::Lost;
  if (percent <= rules.battery_critical_percent) return HealthState::Critical;
  if (percent <= rules.battery_degraded_percent) return HealthState::Degraded;
  return HealthState::Healthy;
}

HealthState aggregate(const std::array<SignalHealth, 7>& signals) {
  HealthState result = HealthState::Healthy;
  for (const auto& signal : signals) {
    if (severity(signal.state) > severity(result)) result = signal.state;
  }
  return result;
}

}  // namespace

std::string_view to_string(HealthState state) {
  switch (state) {
    case HealthState::Healthy: return "HEALTHY";
    case HealthState::Degraded: return "DEGRADED";
    case HealthState::Critical: return "CRITICAL";
    case HealthState::Lost: return "LOST";
  }
  return "UNKNOWN_HEALTH";
}

std::string_view to_string(TelemetrySignal signal) {
  switch (signal) {
    case TelemetrySignal::Heartbeat: return "HEARTBEAT";
    case TelemetrySignal::Gps: return "GPS";
    case TelemetrySignal::Position: return "POSITION";
    case TelemetrySignal::Attitude: return "ATTITUDE";
    case TelemetrySignal::Battery: return "BATTERY";
    case TelemetrySignal::CommandAck: return "COMMAND_ACK";
    case TelemetrySignal::MissionSync: return "MISSION_SYNC";
  }
  return "UNKNOWN_SIGNAL";
}

int severity(HealthState state) { return static_cast<int>(state); }

TelemetryHealthMonitor::TelemetryHealthMonitor(const time::IClock& clock, TelemetryRules rules,
                                               logging::EventLogger& logger)
    : clock_(clock), rules_(rules), logger_(logger) {}

HealthAssessment TelemetryHealthMonitor::evaluate(const TelemetrySnapshot& snapshot) {
  const auto now = clock_.now();
  std::array<SignalHealth, 7> signals{{
      {TelemetrySignal::Heartbeat, freshness_state(now, snapshot.heartbeat, rules_.heartbeat)},
      {TelemetrySignal::Gps, freshness_state(now, snapshot.gps, rules_.gps)},
      {TelemetrySignal::Position, freshness_state(now, snapshot.position, rules_.position)},
      {TelemetrySignal::Attitude, freshness_state(now, snapshot.attitude, rules_.attitude)},
      {TelemetrySignal::Battery, battery_state(snapshot.battery_percent, rules_)},
      {TelemetrySignal::CommandAck, freshness_state(now, snapshot.command_ack, rules_.command_ack)},
      {TelemetrySignal::MissionSync,
       freshness_state(now, snapshot.mission_sync, rules_.mission_sync)},
  }};

  const auto candidate = aggregate(signals);
  if (candidate == HealthState::Healthy && state_ != HealthState::Healthy) {
    ++healthy_streak_;
    if (healthy_streak_ >= std::max(1U, rules_.healthy_samples_for_recovery)) {
      state_ = HealthState::Healthy;
      healthy_streak_ = 0;
    }
  } else {
    healthy_streak_ = 0;
    state_ = candidate;
  }

  logger_.append({now, logging::RecordKind::HealthAssessment, next_record_id_++,
                  mission::MissionState::Boot, mission::MissionState::Boot,
                  std::string(to_string(state_)), true, mission::ReasonCode::None});
  return {signals, state_};
}

HealthState TelemetryHealthMonitor::state() const { return state_; }

}  // namespace uavmv::telemetry
