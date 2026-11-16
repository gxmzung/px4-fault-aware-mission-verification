#ifndef MISSION_STATE_MACHINE_HPP
#define MISSION_STATE_MACHINE_HPP

#include <string>

enum class MissionState {
    IDLE,
    ARMING,
    TAKEOFF,
    MISSION,
    RETURN_HOME,
    LANDING,
    COMPLETE,
    FAILSAFE
};

class MissionStateMachine {
public:
    MissionStateMachine();

    MissionState getState() const;
    std::string getStateName() const;

    void next();
    void triggerFailsafe();
    bool isFinished() const;

private:
    MissionState state;
};

std::string missionStateToString(MissionState state);

#endif