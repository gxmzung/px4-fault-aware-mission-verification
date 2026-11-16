# Mission State Machine C++ — historical source preservation

Original: https://github.com/gxmzung/mission-state-machine-cpp

This directory stores the **15 Git-tracked files from the original main branch** byte-for-byte in `original/`. `SOURCE_SHA1_MANIFEST.txt` pins their original Git blob identifiers. A separate GitHub Actions job checks hash identity and compiles/runs the original standalone C++17 example.

## Not merged into active flight logic

`px4-fault-aware-mission-verification` is a C++20 **event/guard-based research verification platform**, whereas the original demo advances through seven predefined states with `next()` and basic battery/signal/GPS/temperature checks. The two use **different safety assumptions**:

| Component | Legacy demo | UAVMV research model |
|---|---|---|
| Battery threshold | 25% | default degraded 30%, critical 15%, lost 5% |
| Signal | threshold 40 | freshness of telemetry signals; no directly equivalent raw strength threshold |
| GPS | satellite count >=8 | timestamp and validity based GPS health |
| Temperature | <=75 | not the same temperature threshold in the active telemetry rules |
| State flow | fixed `next()` | events, context, transition results and reason codes |

Do **not** substitute legacy failsafe thresholds for the active model.

## Important discovered historical behavior

The original `main.cpp` advances to `COMPLETE` after six safe telemetry samples, then exits its loop; the seventh (22% battery) example is **never processed**, so the sample demo does **not** demonstrate its advertised low-battery failsafe. The standalone regression checks current behavior, not intended safety behavior. The active UAVMV model must not inherit this pitfall.

## Safe archive recommendation

- Original source files, documentation, sample data, Makefile and tests notes preserved (15/15).
- C++17 binary compiles/runs independently; this is not a UAVMV API migration.
- Once this PR is merged and existing CMake/CTest CI passes, original repository is an **Archive candidate**.
- Archive retains the original Git commit graph and URLs. Never delete based solely on file count.
- This is deterministic software simulation only: no PX4, SITL, or real-aircraft integration.
