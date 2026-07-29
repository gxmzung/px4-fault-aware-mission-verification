cmake_minimum_required(VERSION 3.20)
project(px4_fault_aware_mission_verification VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
  Catch2
  URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.8.1.tar.gz
  URL_HASH SHA256=18b3f70ac80fccc340d8c6ff0f339b2ae64944782f8d2fca2bd705cf47cadb79
  DOWNLOAD_EXTRACT_TIMESTAMP FALSE
)
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
  URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
  DOWNLOAD_EXTRACT_TIMESTAMP FALSE
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(uavmv
  src/command/command.cpp
  src/command/command_guard.cpp
  src/logging/event_logger.cpp
  src/mission/mission_state.cpp
  src/mission/mission_state_machine.cpp
  src/telemetry/telemetry_health_monitor.cpp
  src/failsafe/failsafe_supervisor.cpp
  src/faults/fault_injector.cpp
  src/faults/fault_scheduler.cpp
  src/faults/fault_types.cpp
  src/simulation/scenario_loader.cpp
  src/vehicle/virtual_fc.cpp
)
target_include_directories(uavmv PUBLIC include)
target_link_libraries(uavmv PUBLIC nlohmann_json::nlohmann_json)

if(MSVC)
  target_compile_options(uavmv PRIVATE /W4 /permissive-)
else()
  target_compile_options(uavmv PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_executable(uavmv_tests
  tests/unit/test_clock.cpp
  tests/unit/test_command_guard.cpp
  tests/unit/test_mission_state_machine.cpp
  tests/unit/test_virtual_fc.cpp
  tests/unit/test_logging.cpp
  tests/unit/test_telemetry_health_monitor.cpp
  tests/unit/test_failsafe_supervisor.cpp
  tests/unit/test_fault_injector.cpp
  tests/unit/test_fault_scheduler.cpp
  tests/unit/test_scenario_loader.cpp
  tests/integration/test_mission_flow.cpp
  tests/integration/test_health_supervision.cpp
  tests/scenario/test_fault_scenarios.cpp
)
target_link_libraries(uavmv_tests PRIVATE uavmv Catch2::Catch2WithMain)
target_compile_definitions(uavmv_tests PRIVATE UAVMV_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

add_test(
  NAME mission_verification_tests
  COMMAND uavmv_tests --reporter compact --rng-seed 1 --out test-output.txt
)
