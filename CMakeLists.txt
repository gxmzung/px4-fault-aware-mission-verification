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

add_library(uavmv
  src/command/command.cpp
  src/command/command_guard.cpp
  src/logging/event_logger.cpp
  src/mission/mission_state.cpp
  src/mission/mission_state_machine.cpp
  src/telemetry/telemetry_health_monitor.cpp
  src/failsafe/failsafe_supervisor.cpp
  src/vehicle/virtual_fc.cpp
)
target_include_directories(uavmv PUBLIC include)

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
  tests/integration/test_mission_flow.cpp
  tests/integration/test_health_supervision.cpp
)
target_link_libraries(uavmv_tests PRIVATE uavmv Catch2::Catch2WithMain)

add_test(
  NAME phase1_and_phase2_tests
  COMMAND uavmv_tests --reporter compact --rng-seed 1 --out test-output.txt
)
