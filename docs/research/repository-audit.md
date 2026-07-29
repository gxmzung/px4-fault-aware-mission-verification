# Repository Audit and Architecture Decision

**Project:** `px4-fault-aware-mission-verification`  
**Phase:** 0 — Repository audit  
**Audit date:** 2026-07-29  
**Status vocabulary:** `IMPLEMENTED`, `TESTED`, `SIMULATED`, and `PLANNED` are used deliberately.

## 1. Executive assessment

The supplied workspace does not contain an existing repository. It contains two empty directories, `outputs/` and `work/`, and is not inside a Git worktree. There are no source files, build files, tests, scenario definitions, experiment logs, diagrams, reports, CI workflows, or prior results to assess.

Consequently:

- **IMPLEMENTED:** this Phase 0 audit document only.
- **TESTED:** no mission-system component.
- **SIMULATED:** no mission or fault scenario.
- **PLANNED:** all architecture, tests, experiments, PX4/ROS2 integration, and hardware validation described below.

No claim about robustness, safety, reliability, timing, test coverage, PX4 compatibility, ROS2 compatibility, flight behavior, or research results is currently supported.

The absence of legacy code is useful in one respect: the project can begin with clean boundaries and traceability between requirements, tests, scenarios, metrics, and research claims. It also means Phase 1 must establish the build, interfaces, and evidence discipline before broad feature development.

## 2. Inspection scope and inventory

The audit included:

- recursive listing of normal and hidden files;
- inspection of the top-level directory;
- verification of Git worktree status;
- inspection for source, tests, documentation, experiment artifacts, and configuration.

Observed inventory before this audit was written:

```text
workspace/
├── outputs/    # empty
└── work/       # empty
```

Git reported that the workspace was not a repository.

### Reusable components

None. No implementation was present.

### Weak or demo-only code

None was present to evaluate.

### Duplicate or misleading code

None was present to evaluate.

### Unsupported claims

No repository claims were present. Any future README or report must avoid presenting planned behavior as implemented behavior and must derive quantitative statements from committed logs or reproducible generated artifacts.

## 3. Technical debt and evidence gaps

There is no inherited code debt, but the project begins with the following evidence gaps:

1. No deterministic mission model or documented transition table.
2. No command validation boundary or command-result model.
3. No virtual vehicle contract against which mission behavior can be tested.
4. No telemetry freshness/validity model with deterministic thresholds.
5. No configurable failsafe policy or multi-fault precedence rule.
6. No repeatable fault scheduler or machine-readable scenario schema.
7. No structured event log or stable experiment record schema.
8. No baseline implementation for controlled comparison.
9. No unit, integration, or end-to-end scenario tests.
10. No CI, coverage report, or reproducibility command.
11. No experiment runs, raw logs, summaries, or plots.
12. No verified PX4 SITL, ROS2, Gazebo, HITL, or hardware integration.

These are missing capabilities, not defects in existing code.

## 4. Research positioning

### Research question

> Can an explicit mission-state architecture with command validation, telemetry-health monitoring, fault injection, and failsafe supervision make autonomous UAV mission software easier to verify and more robust against realistic mission failures?

The phrase “more robust” must be operationalized. For this project it should mean measurable improvement over a declared baseline in one or more of:

- correct detection of injected faults;
- bounded detection latency;
- correct mission-level failsafe selection;
- rejection of unsafe or context-invalid commands;
- preservation of valid state transitions;
- recovery after transient faults;
- mission completion when completion remains safe under the scenario;
- avoidance of false-positive failsafe transitions.

The project does not propose a new autopilot or low-level controller. PX4 remains responsible for stabilization, estimation, actuator control, and its native safety mechanisms. The research subject is the testability and behavior of a higher-level mission-software layer.

### Modest intended contributions

- **C1 — Architecture:** a modular high-level mission architecture separating mission logic, command validation, telemetry health, vehicle I/O, and failsafe policy.
- **C2 — Verification framework:** deterministic, repeatable mission-level fault injection with explicit oracles.
- **C3 — Measurement:** metrics and logs that quantify mission-level response without claiming control-theoretic novelty.
- **C4 — Integration path:** an adapter boundary that permits later PX4 SITL and ROS2 integration without making either a prerequisite for the virtual experiments.

These are research aims, not completed contributions.

## 5. Proposed architecture

### Architectural boundaries

```text
Mission plan
    |
    v
Mission State Machine <--------- Failsafe Supervisor
    |                                    ^
    v                                    |
Command Guard                       Health events
    |                                    |
    v                                    |
Vehicle Adapter <------------- Telemetry Health Monitor
    |                                    ^
    v                                    |
Virtual FC (Stage 1) -------------- Telemetry
PX4 Adapter (planned)

Fault Scheduler ---> fault ports in telemetry, ACK, mission sync,
                     heartbeat, vehicle state, and sensor validity

All components ---> structured event sink ---> metrics and experiment logs
```

### Dependency rule

Core policy code must depend on domain types and narrow interfaces, not on PX4, ROS2, a simulator, wall-clock time, or filesystem I/O. Time should enter through a monotonic clock abstraction or explicit timestamps so tests remain deterministic.

The vehicle adapter translates between mission-level commands/telemetry and a backend. The virtual FC is the only backend in the initial experiment stage. A future PX4 adapter must implement the same contract rather than leaking PX4 message types into mission policy.

### Module responsibilities

| Module | Responsibility | Must not own |
|---|---|---|
| `mission` | states, events, transition table, context, transition records | vehicle transport or health classification |
| `command` | preconditions, duplicate/rate checks, timeout and ACK outcomes | low-level PX4 commander behavior |
| `telemetry` | snapshot model, freshness rules, per-signal and aggregate health | mission-state transitions |
| `failsafe` | policy selection, severity precedence, retry/escalation decisions | telemetry parsing or command transport |
| `vehicle` | backend-neutral adapter and deterministic virtual FC | research policy |
| `faults` | scheduled, repeatable perturbations at named injection points | hidden random behavior |
| `metrics` | event schema, counters, latency derivation, run summaries | policy decisions |
| `simulation` | deterministic clock, scenario execution, baseline/system variants | hard-coded scenario expectations |

### Proposed repository layout

```text
px4-fault-aware-mission-verification/
├── CMakeLists.txt
├── cmake/
├── include/famv/
│   ├── mission/
│   ├── command/
│   ├── telemetry/
│   ├── failsafe/
│   ├── vehicle/
│   ├── faults/
│   ├── metrics/
│   └── simulation/
├── src/
│   ├── mission/
│   ├── command/
│   ├── telemetry/
│   ├── failsafe/
│   ├── vehicle/
│   ├── faults/
│   ├── metrics/
│   └── simulation/
├── apps/
│   └── scenario_runner.cpp
├── tests/
│   ├── unit/
│   ├── integration/
│   ├── scenario/
│   └── fixtures/
├── experiments/
│   ├── configs/
│   ├── results/
│   │   └── README.md
│   └── plots/
│       └── README.md
├── tools/
│   ├── run_experiments.py
│   ├── analyze_results.py
│   └── plot_results.py
├── docs/
│   ├── architecture/
│   ├── experiments/
│   ├── research/
│   │   └── repository-audit.md
│   └── diagrams/
├── paper/
│   └── technical_report.md
└── .github/workflows/
    └── ci.yml
```

Public headers are separated from implementation files. The namespace abbreviation `famv` avoids a long namespace without obscuring the repository name in documentation. Exact naming can be revisited in Phase 1 before interfaces stabilize.

### Core domain decisions

1. Use strongly typed enums for mission states, mission events, health states, commands, outcomes, fault types, and severity.
2. Express valid mission transitions as reviewable table data, with optional guards and actions, rather than a large switch with implicit fall-through behavior.
3. Return structured decisions (`accepted/rejected`, reason code, retry information) instead of booleans.
4. Record every accepted and rejected transition and command decision.
5. Use injected time and seeded randomness. The default scenario runner should be deterministic.
6. Keep fault definitions external in YAML. Validate them against a documented schema before execution.
7. Treat expected behavior as an experiment oracle separate from the injected stimulus.
8. Keep baseline and proposed-system variants in one runner so they receive identical scenario timing and seeds.

## 6. Transition and safety model to specify in Phase 1

The initial state set should be:

`BOOT`, `PREFLIGHT`, `ARMED`, `TAKEOFF`, `MISSION`, `HOLD`, `RETURN_TO_HOME`, `LAND`, `RECOVERY`, `FAILSAFE`, `COMPLETED`, and `ABORTED`.

Before implementation, Phase 1 should create a normative transition table containing:

- current state;
- accepted event;
- optional guard;
- next state;
- reason code;
- emitted action;
- whether the transition is terminal;
- associated test identifier.

No “any state to any state” shortcut should be added except a narrowly documented operator-abort rule if analysis justifies it. Invalid events must produce a logged rejection without mutating state.

The command guard should validate only high-level mission safety preconditions. It must not claim to replace PX4 commander checks. Proposed initial guarded commands are arm, takeoff, start mission, hold, return-to-home, land, and disarm.

## 7. Fault model and scenario contract

Each scenario should include:

- stable scenario ID and schema version;
- deterministic seed;
- mission setup and run duration;
- system variant (`baseline_a` or `system_b`);
- fault type, injection point, start, duration, and severity;
- explicit expected events/transitions;
- detection and recovery deadlines where applicable;
- prohibited transitions or commands;
- terminal pass/fail criteria.

Initial scenario set:

| ID | Fault | Primary experiment oracle |
|---|---|---|
| F01 | telemetry delay | degradation/loss detected within configured bound |
| F02 | telemetry packet loss | classification follows deterministic loss rule |
| F03 | complete telemetry loss | configured hold/RTH/land policy selected |
| F04 | GPS degradation | no mission continuation contrary to policy |
| F05 | GPS dropout | critical/lost classification and configured response |
| F06 | command rejection | rejection recorded; retry/escalation bounded |
| F07 | command ACK timeout | timeout detected without duplicate dangerous command |
| F08 | mission upload mismatch | mission start rejected |
| F09 | offboard heartbeat loss | fallback policy selected |
| F10 | critical battery | configured terminal safety action selected |
| F11 | telemetry and GPS degradation | highest-severity deterministic policy wins |
| F12 | transient fault and recovery | recovery criteria and time are measured |

Threshold values must be configuration parameters justified in documentation. Initial values used by the virtual system are test assumptions, not validated flight thresholds.

## 8. Experiment plan

### Compared systems

**Baseline A — minimal mission executor:** explicit state machine and vehicle adapter, but no telemetry-health-driven supervisor and no proactive command guard beyond what is required to keep the runner executable. Any retained validation must be enumerated. This baseline represents a deliberately simplified high-level mission application, not stock PX4 and not the state of the art.

**System B — fault-aware architecture:** the same mission, virtual FC, scenario timing, and seed, with telemetry health monitoring, command guard, and configurable failsafe supervisor enabled.

The shared runner and backend reduce confounding differences. Variant configuration and component enablement must appear in each run manifest.

### Independent variables

- system variant;
- fault scenario;
- severity/duration where defined;
- deterministic seed when stochastic packet loss is used.

### Dependent metrics

| Metric | Operational definition |
|---|---|
| Mission completion rate | completed runs divided by eligible runs; scenarios where abort is the correct outcome must be reported separately |
| Fault detection rate | injected detectable faults with the expected detection event divided by eligible injected faults |
| Detection latency | first matching detection timestamp minus fault activation timestamp |
| Correct failsafe transition rate | runs reaching the scenario-specified transition within its deadline |
| Unsafe command rejection rate | prohibited injected command attempts rejected by the guard |
| Recovery success rate | recoverable runs meeting the declared recovery oracle |
| Recovery time | recovery-confirmed timestamp minus fault-clear timestamp |
| False-positive failsafe rate | failsafe actions occurring without an active/qualifying fault |
| Invalid transition count | rejected transition attempts, reported by cause; not silently treated as success |
| State consistency | runs whose transition sequence is accepted by the normative transition model |

Test count and code coverage are engineering evidence, not experimental outcome metrics.

### Repetition strategy

Deterministic cases need one execution per exact configuration in CI; repeated identical deterministic runs add little evidence. Scenarios with seeded packet loss or timing variation should use a preregistered seed set and enough repetitions to report distributions or confidence intervals. The run count will be chosen and documented before results are interpreted. No target such as “1,200 runs” should be reported as completed until logs exist.

### Logging and provenance

Each run should write:

1. `manifest.json` — commit identifier if available, build type, configuration hash, seed, system variant, scenario ID, and tool versions;
2. `events.jsonl` — state, health, command, fault, and supervisor decisions with simulation and monotonic timestamps;
3. `summary.json` — oracle outcome and derived metrics;
4. optional console log for diagnostics, not used as the primary data source.

Analysis scripts must consume raw logs, never embedded example numbers. Generated plots should identify their source manifest(s).

### Analysis sequence

1. Validate manifests and log schemas.
2. Check scenario oracle eligibility.
3. Compute per-run metrics.
4. Aggregate by scenario and system variant.
5. Report counts and missing/invalid runs.
6. Plot baseline versus System B only where the metric is defined for both.
7. Preserve raw logs and generated summaries needed to reproduce report tables.

## 9. Test strategy

Tests should grow from risk and requirements, not from a numerical quota.

### Unit tests

- every allowed transition and representative invalid events from every state;
- transition record contents and no mutation on rejection;
- command preconditions, duplicate suppression, rate limits, timeout, retry, and ACK rejection;
- boundary timestamps for every telemetry freshness threshold;
- aggregate health precedence;
- failsafe policy selection, ties, simultaneous faults, retry exhaustion, and recovery hysteresis;
- scenario schema validation and scheduler boundary timing;
- metric calculations from small known event sequences.

### Integration tests

- telemetry snapshot → health event → supervisor decision → mission transition;
- command guard → adapter → ACK/timeout result;
- injected fault → altered virtual signal → expected detection;
- structured logging and replay;
- simultaneous faults and deterministic precedence.

### Scenario tests

- nominal mission without injected faults;
- F01–F12, including expected response and forbidden behavior;
- baseline and System B using identical inputs;
- deterministic replay using recorded configuration and seed.

The initial “80 tests” is a planning target, not a requirement to manufacture cases. The README should obtain the current count from actual test discovery/output after tests exist.

## 10. Files to keep, delete, or refactor

### Keep

- `docs/research/repository-audit.md` — this Phase 0 record.

No pre-existing implementation files exist to keep.

### Delete

None. The pre-audit workspace contained no files.

### Refactor

None. The pre-audit workspace contained no code.

### Create later, after approval

The layout in Section 5 should be introduced incrementally. Phase 1 should create only the build, core domain modules, virtual adapter, logger, and their tests. Empty placeholder trees and result files should be avoided unless they communicate a necessary provenance rule.

## 11. Staged implementation plan

### Milestone 0 — Audit and architecture decision

**Deliverable:** this document.  
**Exit criterion:** research scope, architecture, baseline, experiment variables, and evidence language are approved.

### Milestone 1 — Deterministic mission core

Implement the build system, domain types, documented transition table, mission context, command guard, vehicle interface, deterministic virtual FC, and JSONL event records.

**Experiment enabled:** nominal missions plus command/state violation trials.  
**Claims potentially supported:** explicit transitions and high-level command rules are mechanically testable.  
**Exit criteria:** all transition rules and guard decisions have tests; a nominal virtual mission and invalid-command scenario produce reproducible logs.

### Milestone 2 — Health and failsafe policy

Implement timestamped telemetry snapshots, deterministic rule evaluation, aggregate health, configurable policy selection, escalation, and recovery hysteresis.

**Experiment enabled:** health threshold boundary tests and supervised versus unsupervised response comparisons.  
**Claims potentially supported:** monitoring and policy are separable and their decisions are traceable.  
**Exit criteria:** rule boundaries, simultaneous signals, and policy precedence are tested without simulator wall-clock dependence.

### Milestone 3 — Fault injection and scenario schema

Implement YAML validation, deterministic scheduler, named injection points, F01–F12 configurations, and explicit scenario oracles.

**Experiment enabled:** repeatable campaign across declared fault types.  
**Claims potentially supported:** failures and expected behavior can be reproduced from configuration.  
**Exit criteria:** every scenario is schema-valid, deterministic for a seed, and has at least one end-to-end oracle test.

### Milestone 4 — Verification suite and CI

Complete requirement-linked unit/integration/scenario tests, sanitizer-capable builds where supported, CI configuration, and coverage reporting.

**Experiment enabled:** regression verification on each change.  
**Claims potentially supported:** enumerated requirements are continuously checked on the supported build environment.  
**Exit criteria:** clean configure/build/test from documented commands; CI runs the same test classes; actual test count and coverage are reported without fixed marketing targets.

### Milestone 5 — Comparative experiments

Implement the precisely documented baseline, batch runner, manifests, metric derivation, data validation, and plotting.

**Experiment enabled:** paired Baseline A/System B evaluation under identical scenarios and seeds.  
**Claims potentially supported:** only statistically and operationally supported differences in the defined metrics.  
**Exit criteria:** clean reproduction command generates raw logs, summaries, and plots; failed or missing runs are visible; no hand-entered result values.

### Milestone 6 — Documentation

Write the professor-facing README, transition table, policy rationale, schema reference, architecture/state/sequence/pipeline Mermaid sources, reproducibility guide, and limitations.

**Exit criterion:** a reader can distinguish implemented, tested, simulated, and planned functionality and trace every quantitative statement to artifacts.

### Milestone 7 — Undergraduate technical report

Write `paper/technical_report.md` from the actual implementation and experiment artifacts. Use verified references or explicit TODOs.

**Exit criterion:** results and discussion match generated summaries, threats to validity are stated, and the document does not imply peer review or flight validation.

### Milestone 8 — PX4/ROS2 integration

Proceed only after the virtual framework is stable:

1. PX4 SITL adapter;
2. ROS2 integration;
3. PX4 SITL with Gazebo;
4. HITL;
5. Pixhawk and companion-computer bench integration;
6. controlled flight experiments subject to appropriate safety review.

Each stage needs separate evidence and must not inherit “verified” status from the virtual FC.

## 12. Risks and design controls

| Risk | Control |
|---|---|
| Reimplementing PX4 safety logic | keep guard scope mission-level and document the boundary |
| Baseline designed to fail artificially | define minimal retained behavior before running experiments; apply identical vehicle/scenario inputs |
| Simulator-only conclusions generalized to flight | label virtual results and state external-validity limits |
| Thresholds appear authoritative | treat them as configurable experimental assumptions pending PX4/hardware validation |
| Metrics reward unnecessary mission completion | define scenario-specific safe terminal outcomes |
| Fault injection bypasses real interfaces unrealistically | name injection points and later map each to PX4/ROS2 mechanisms |
| Timing claims depend on host scheduling | use simulation time initially and distinguish it from wall-clock/real-time behavior |
| Results cannot be reproduced | store manifests, seeds, raw logs, schemas, and analysis commands |
| Architecture becomes framework-heavy | add interfaces only at independently testable or replaceable boundaries |

## 13. Current integration and validation status

| Stage | Status | Evidence |
|---|---|---|
| Phase 0 repository audit | IMPLEMENTED | this document |
| Deterministic virtual FC | PLANNED | none |
| Mission tests | PLANNED | none |
| Fault experiments | PLANNED | none |
| PX4 SITL adapter | PLANNED | none |
| ROS2 integration | PLANNED | none |
| Gazebo simulation | PLANNED | none |
| HITL | PLANNED | none |
| Pixhawk/Jetson bench test | PLANNED | none |
| Real UAV flight validation | NOT PERFORMED | no hardware logs or flight data |

## 14. Phase 0 decision request

Before Phase 1 begins, approval is requested for:

1. C++ as the core language with CMake and Python limited to orchestration/analysis;
2. the module boundaries and dependency rule in Section 5;
3. a deterministic virtual FC as the only initial backend;
4. Baseline A as a minimal mission executor, explicitly not PX4;
5. YAML scenario definitions and JSON/JSONL evidence artifacts;
6. the operational metrics and milestone exit criteria above.

No Phase 1 code, build scaffolding, experiment data, or results have been created as part of this audit.

## 15. Phase 1 completion note

**Completed:** 2026-07-29

Phase 1 was approved and implemented after this audit. The repository now contains a C++20/CMake mission core with explicit transition rules, a small mission context, structured reason codes, high-level command validation, a narrow vehicle-adapter interface, a deterministic minimal `VirtualFC`, clock abstraction with manual test time, structured event records with JSONL serialization, Catch2 unit/integration tests, Phase 1 documentation, and an Ubuntu GitHub Actions workflow.

This completion note does not revise the original pre-implementation inventory in Sections 1–3. No telemetry-health monitor, failsafe supervisor, fault injector, scenario configuration, experiment results, PX4/ROS2 integration, simulator integration, hardware validation, or flight testing was added in Phase 1. Build and test evidence is reported from the actual local CMake/CTest run and must not be inferred from this note alone.

## 16. Phase 2 completion note

**Completed:** 2026-07-30

Phase 2 adds timestamped telemetry snapshots, configurable freshness/battery rules, explicit four-level signal and aggregate health, deterministic highest-severity precedence, healthy-sample recovery hysteresis, configurable mission-level failsafe actions, and structured health/failsafe records. The complete Phase 1–2 runner passes 54 test cases and 126 assertions in the recorded local run.

Default thresholds and policies are software-test assumptions only. Phase 2 did not add fault injection, YAML scenarios, experiment results, PX4/ROS2 integration, dynamics, hardware validation, or flight testing.

## 17. Phase 3 completion note

**Completed:** 2026-07-30

Phase 3 adds typed fault definitions, deterministic half-open scheduling, composable telemetry/command effects, a versioned JSON loader with validation, a dedicated offboard-heartbeat health signal, and twelve external configurations covering F01–F12. Scenario tests compose the scheduler and injector with the Phase 2 monitor and supervisor.

The complete Phase 1–3 local runner passes 71 test cases and 180 assertions. No batch experiment runner, statistical result, plot, PX4/ROS2 integration, hardware validation, or flight testing was added. Scenario deadlines and expected actions are declared test oracles; quantitative detection-latency experiments remain pending.
