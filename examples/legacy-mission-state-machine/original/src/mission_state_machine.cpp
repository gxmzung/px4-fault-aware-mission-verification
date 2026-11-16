#include "mission_state_machine.hpp"

MissionStateMachine::MissionStateMachine() : state(MissionState::IDLE) {}

MissionState MissionStateMachine::getState() const {
    return state;
}

std::string MissionStateMachine::getStateName() const {
    return missionStateToString(state);
}

void MissionStateMachine::next() {
    switch (state) {
        case MissionState::IDLE:
            state = MissionState::ARMING;
            break;
        case MissionState::ARMING:
            state = MissionState::TAKEOFF;
            break;
        case MissionState::TAKEOFF:
            state = MissionState::MISSION;
            break;
        case MissionState::MISSION:
            state = MissionState::RETURN_HOME;
            break;
        case MissionState::RETURN_HOME:
            state = MissionState::LANDING;
            break;
        case MissionState::LANDING:
            state = MissionState::COMPLETE;
            break;
        case MissionState::COMPLETE:
        case MissionState::FAILSAFE:
            break;
    }
}

void MissionStateMachine::triggerFailsafe() {
    state = MissionState::FAILSAFE;
}

bool MissionStateMachine::isFinished() const {
    return state == MissionState::COMPLETE || state == MissionState::FAILSAFE;
}

std::string missionStateToString(MissionState state) {
    switch (state) {
        case MissionState::IDLE:
            return "IDLE";
        case MissionState::ARMING:
            return "ARMING";
        case MissionState::TAKEOFF:
            return "TAKEOFF";
        case MissionState::MISSION:
            return "MISSION";
        case MissionState::RETURN_HOME:
            return "RETURN_HOME";
        case MissionState::LANDING:
            return "LANDING";
        case MissionState::COMPLETE:
            return "COMPLETE";
        case MissionState::FAILSAFE:
            return "FAILSAFE";
        default:
            return "UNKNOWN";
    }
}