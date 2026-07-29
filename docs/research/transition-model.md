# Phase 1 Mission Transition Model

## State definitions

| State | Phase 1 meaning |
|---|---|
| `BOOT` | mission software has not received system-ready confirmation |
| `PREFLIGHT` | readiness checks and mission preparation may occur |
| `ARMED` | high-level mission model has received arm confirmation |
| `TAKEOFF` | takeoff command has been confirmed; mission start is pending |
| `MISSION` | simplified mission execution |
| `HOLD` | mission progression is paused |
| `RETURN_TO_HOME` | high-level return sequence requested |
| `LAND` | landing sequence requested |
| `RECOVERY` | reserved state for a later recovery policy |
| `FAILSAFE` | reserved state for a later failsafe supervisor |
| `COMPLETED` | terminal successful software sequence |
| `ABORTED` | terminal operator-aborted sequence |

`RECOVERY` and `FAILSAFE` are declared to stabilize the domain vocabulary. Phase 1 does not detect conditions that enter them or implement a supervisor.

## Event definitions

| Event | Meaning |
|---|---|
| `SYSTEM_READY` | mission software prerequisites are available |
| `PREFLIGHT_OK` | preflight readiness is valid |
| `ARM_CONFIRMED` | vehicle-side arm acknowledgement has been observed |
| `TAKEOFF_CONFIRMED` | takeoff command has been acknowledged |
| `WAYPOINT_REACHED` | initial or subsequent simplified waypoint progress |
| `MISSION_COMPLETE` | mission or landing sequence reports completion |
| `HOLD_REQUESTED` | higher-level caller requests hold |
| `RTH_REQUESTED` | higher-level caller requests return-to-home |
| `LAND_REQUESTED` | higher-level caller requests landing |
| `RECOVERY_SUCCESS` | future recovery procedure reports success |
| `RECOVERY_FAILED` | future recovery procedure reports failure |
| `OPERATOR_ABORT` | operator requests terminal abort |

The state machine consumes events; it does not detect telemetry or faults.

## Explicit transition table

| Current state | Event | Next state | Additional rule |
|---|---|---|---|
| `BOOT` | `SYSTEM_READY` | `PREFLIGHT` | — |
| `PREFLIGHT` | `PREFLIGHT_OK` | `PREFLIGHT` | sets preflight-ready context |
| `PREFLIGHT` | `ARM_CONFIRMED` | `ARMED` | preflight-ready must be true |
| `ARMED` | `TAKEOFF_CONFIRMED` | `TAKEOFF` | — |
| `TAKEOFF` | `WAYPOINT_REACHED` | `MISSION` | represents entry into mission |
| `MISSION` | `WAYPOINT_REACHED` | `MISSION` | records progress without a state change |
| `MISSION` | `MISSION_COMPLETE` | `RETURN_TO_HOME` | nominal completion policy for Phase 1 |
| `MISSION` | `HOLD_REQUESTED` | `HOLD` | — |
| `HOLD` | `WAYPOINT_REACHED` | `MISSION` | simplified resume event |
| `TAKEOFF` | `RTH_REQUESTED` | `RETURN_TO_HOME` | — |
| `MISSION` | `RTH_REQUESTED` | `RETURN_TO_HOME` | — |
| `HOLD` | `RTH_REQUESTED` | `RETURN_TO_HOME` | — |
| `ARMED` | `LAND_REQUESTED` | `LAND` | — |
| `TAKEOFF` | `LAND_REQUESTED` | `LAND` | — |
| `MISSION` | `LAND_REQUESTED` | `LAND` | — |
| `HOLD` | `LAND_REQUESTED` | `LAND` | — |
| `RETURN_TO_HOME` | `LAND_REQUESTED` | `LAND` | — |
| `LAND` | `MISSION_COMPLETE` | `COMPLETED` | clears armed context |
| `RECOVERY` | `RECOVERY_SUCCESS` | `HOLD` | entry to recovery is deferred |
| `RECOVERY` | `RECOVERY_FAILED` | `FAILSAFE` | policy is deferred |
| `FAILSAFE` | `LAND_REQUESTED` | `LAND` | caller-selected action only |
| any nonterminal state | `OPERATOR_ABORT` | `ABORTED` | records `OPERATOR_ABORT` reason and clears armed context |

## Invalid transition policy

Any state/event pair absent from the table is rejected. Rejection:

1. returns `accepted = false`;
2. returns `INVALID_STATE_TRANSITION`, or a more specific reason such as `PREFLIGHT_NOT_READY`;
3. leaves mission state unchanged;
4. appends a structured rejected-transition record.

Events received in `COMPLETED` or `ABORTED` are rejected with `TERMINAL_MISSION_STATE`, including a repeated operator abort.

## Scope note

This table expresses Phase 1 research-software semantics. It is not derived from PX4 commander internals and must not be interpreted as an autopilot safety specification.
