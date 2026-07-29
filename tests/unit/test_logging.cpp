#include <catch2/catch_test_macros.hpp>

#include "uavmv/logging/event_logger.hpp"

TEST_CASE("event record serializes as machine-readable JSON") {
  const uavmv::logging::EventRecord record{
      uavmv::time::TimePoint{std::chrono::milliseconds{12}},
      uavmv::logging::RecordKind::MissionTransition,
      7,
      uavmv::mission::MissionState::Boot,
      uavmv::mission::MissionState::Preflight,
      "SYSTEM_READY",
      true,
      uavmv::mission::ReasonCode::None};
  const auto json = uavmv::logging::to_json(record);
  REQUIRE(json.find("\"timestamp_ms\":12") != std::string::npos);
  REQUIRE(json.find("\"accepted\":true") != std::string::npos);
  REQUIRE(json.find("\"next_state\":\"PREFLIGHT\"") != std::string::npos);
}

TEST_CASE("logger clear removes in-memory records") {
  uavmv::logging::EventLogger logger;
  logger.append({});
  logger.clear();
  REQUIRE(logger.records().empty());
}
