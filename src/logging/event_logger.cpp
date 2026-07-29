#include "uavmv/logging/event_logger.hpp"

#include <fstream>
#include <stdexcept>

namespace uavmv::logging {
namespace {

std::string escape_json(std::string_view value) {
  std::string result;
  result.reserve(value.size());
  for (const char character : value) {
    switch (character) {
      case '\\': result += "\\\\"; break;
      case '"': result += "\\\""; break;
      case '\n': result += "\\n"; break;
      case '\r': result += "\\r"; break;
      case '\t': result += "\\t"; break;
      default: result += character; break;
    }
  }
  return result;
}

}  // namespace

std::string_view to_string(RecordKind kind) {
  switch (kind) {
    case RecordKind::MissionTransition: return "MISSION_TRANSITION";
    case RecordKind::CommandDecision: return "COMMAND_DECISION";
    case RecordKind::VehicleCommand: return "VEHICLE_COMMAND";
    case RecordKind::HealthAssessment: return "HEALTH_ASSESSMENT";
    case RecordKind::FailsafeDecision: return "FAILSAFE_DECISION";
  }
  return "UNKNOWN_RECORD";
}

std::string to_json(const EventRecord& record) {
  const auto timestamp_ms = record.timestamp.time_since_epoch().count();
  return "{\"timestamp_ms\":" + std::to_string(timestamp_ms) +
         ",\"kind\":\"" + std::string(to_string(record.kind)) +
         "\",\"record_id\":" + std::to_string(record.record_id) +
         ",\"previous_state\":\"" +
         std::string(mission::to_string(record.previous_state)) +
         "\",\"next_state\":\"" +
         std::string(mission::to_string(record.next_state)) +
         "\",\"subject\":\"" + escape_json(record.subject) +
         "\",\"accepted\":" + (record.accepted ? "true" : "false") +
         ",\"reason\":\"" + std::string(mission::to_string(record.reason)) + "\"}";
}

void EventLogger::append(EventRecord record) { records_.push_back(std::move(record)); }

const std::vector<EventRecord>& EventLogger::records() const { return records_; }

void EventLogger::clear() { records_.clear(); }

void EventLogger::write_jsonl(const std::filesystem::path& path) const {
  std::ofstream output(path);
  if (!output) {
    throw std::runtime_error("unable to open JSONL output");
  }
  for (const auto& record : records_) {
    output << to_json(record) << '\n';
  }
  if (!output) {
    throw std::runtime_error("unable to write JSONL output");
  }
}

}  // namespace uavmv::logging
