#include <iostream>
#include "mission_logger.hpp"

void MissionLogger::logState(const std::string& stateName) const {
    std::cout << "[STATE] " << stateName << std::endl;
}

void MissionLogger::logWarning(const std::string& message) const {
    std::cout << "[WARNING] " << message << std::endl;
}

void MissionLogger::logEvent(const std::string& message) const {
    std::cout << "[EVENT] " << message << std::endl;
}