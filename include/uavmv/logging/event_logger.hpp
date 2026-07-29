#pragma once

#include <filesystem>
#include <vector>

#include "uavmv/logging/event_record.hpp"

namespace uavmv::logging {

class EventLogger {
 public:
  void append(EventRecord record);
  [[nodiscard]] const std::vector<EventRecord>& records() const;
  void clear();
  void write_jsonl(const std::filesystem::path& path) const;

 private:
  std::vector<EventRecord> records_;
};

}  // namespace uavmv::logging
