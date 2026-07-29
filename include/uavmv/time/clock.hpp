#pragma once

#include <chrono>

namespace uavmv::time {

using Duration = std::chrono::milliseconds;
using TimePoint = std::chrono::time_point<std::chrono::steady_clock, Duration>;

class IClock {
 public:
  virtual ~IClock() = default;
  [[nodiscard]] virtual TimePoint now() const = 0;
};

class SteadyClock final : public IClock {
 public:
  [[nodiscard]] TimePoint now() const override {
    return std::chrono::time_point_cast<Duration>(std::chrono::steady_clock::now());
  }
};

class ManualClock final : public IClock {
 public:
  explicit ManualClock(TimePoint initial = TimePoint{}) : current_(initial) {}

  [[nodiscard]] TimePoint now() const override { return current_; }
  void advance(Duration amount) { current_ += amount; }
  void set(TimePoint value) { current_ = value; }

 private:
  TimePoint current_;
};

}  // namespace uavmv::time
