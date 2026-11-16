# Mission State Machine C++

C++ mission state machine for autonomous systems, telemetry checks, and failsafe transitions.

This project models a simplified mission-control state machine used in drones, robots, field devices, and mission-critical embedded systems.

## Mission Flow

```text
IDLE
  -> ARMING
  -> TAKEOFF
  -> MISSION
  -> RETURN_HOME
  -> LANDING
  -> COMPLETE
Failsafe can interrupt the mission when telemetry health becomes unsafe.

Goals
Implement mission state transitions in C++
Validate telemetry health before state changes
Simulate failsafe conditions
Practice autonomous-system control logic
Build readable C++ architecture for mission software
Project Structure
src/
  main.cpp
  mission_state_machine.cpp
  telemetry_health.cpp
  failsafe_manager.cpp
  mission_logger.cpp

include/
  mission_state_machine.hpp
  telemetry_health.hpp
  failsafe_manager.hpp
  mission_logger.hpp

docs/
  mission_flow.md
  roadmap.md

data/
  sample_mission_events.txt

tests/
  state_transition_notes.md
Target Position

Autonomous systems, embedded mission software, ROS2/PX4-style control flow, telemetry diagnostics, and field operation systems.

Status

Early implementation.