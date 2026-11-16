#ifndef MISSION_LOGGER_HPP
#define MISSION_LOGGER_HPP

#include <string>

class MissionLogger {
public:
    void logState(const std::string& stateName) const;
    void logWarning(const std::string& message) const;
    void logEvent(const std::string& message) const;
};

#endif