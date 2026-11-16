#include <iostream>
#include <vector>
#include "mission_state_machine.hpp"
#include "telemetry_health.hpp"
#include "failsafe_manager.hpp"
#include "mission_logger.hpp"

int main() {
    MissionStateMachine mission;
    FailsafeManager failsafe;
    MissionLogger logger;

    std::vector<TelemetryHealth> telemetryTimeline = {
        {95.0, 88.0, 14.0, 42.0},
        {88.0, 82.0, 13.0, 45.0},
        {74.0, 76.0, 12.0, 49.0},
        {62.0, 70.0, 11.0, 54.0},
        {48.0, 64.0, 10.0, 58.0},
        {31.0, 51.0, 9.0, 62.0},
        {22.0, 45.0, 8.0, 66.0}
    };

    logger.logEvent("Mission simulation started");

    for (const auto& health : telemetryTimeline) {
        logger.logState(mission.getStateName());

        if (failsafe.shouldTriggerFailsafe(health)) {
            std::string reason = failsafe.getFailsafeReason(health);
            logger.logWarning("Failsafe triggered: " + reason);
            mission.triggerFailsafe();
            break;
        }

        mission.next();

        if (mission.isFinished()) {
            break;
        }
    }

    logger.logState(mission.getStateName());
    logger.logEvent("Mission simulation finished");

    return 0;
}